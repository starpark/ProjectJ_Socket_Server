#include "pch.h"
#include "SessionBase.h"
#include "Service.h"
#include "SocketUtils.h"

SessionBase::SessionBase()
{
	socket_ = SocketUtils::CreateSocket();
}

SessionBase::~SessionBase()
{
	closesocket(socket_);
	//cout << "~SessionBase" << endl;
}

void SessionBase::ProcessCompletePort(OverlappedEx* overlapped, unsigned int numOfBytes)
{
	switch (overlapped->eventType_)
	{
	case EventType::CONNECT:
		ProcessConnect();
		break;
	case EventType::DISCONNECT:
		ProcessDisconnect();
		break;
	case EventType::RECV:
		ProcessRecv(numOfBytes);
		break;
	case EventType::SEND:
		ProcessSend(numOfBytes);
		break;
	default:
		Disconnect();
		break;
	}
}

bool SessionBase::Connect()
{
	return RegisterConnect();
}

void SessionBase::Disconnect()
{
	if (bIsConnected_.exchange(false) == false)
	{
		return;
	}

	RegisterDisconnect();
}


void SessionBase::AcceptAndConnect()
{
	ProcessConnect();
}

void SessionBase::Send(shared_ptr<SendBuffer> sendBuffer)
{
	if (IsConnected() == false)
	{
		return;
	}

	bool isRegistered = false;

	{
		WRITE_LOCK;

		sendQueue_.push(sendBuffer);

		if (bIsRegisteredSend_.exchange(true) == false)
		{
			isRegistered = true;
		}
	}

	if (isRegistered)
	{
		RegisterSend();
	}
}

bool SessionBase::RegisterConnect()
{
	if (IsConnected())
	{
		return false;
	}

	if (GetOwnerService()->GetServiceType() != ServiceType::CLIENT)
	{
		return false;
	}

	if (SocketUtils::SetReuseAddress(socket_, true) == false)
	{
		return false;
	}

	if (SocketUtils::BindAnyAddress(socket_, 0) == false)
	{
		return false;
	}

	connectEvent_.Init();
	connectEvent_.owner_ = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetOwnerService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(socket_, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0,
	                                    &numOfBytes, &connectEvent_))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			connectEvent_.owner_ = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

void SessionBase::RegisterDisconnect()
{
	disconnectEvent_.Init();
	disconnectEvent_.owner_ = shared_from_this();

	if (false == SocketUtils::DisconnectEx(socket_, &disconnectEvent_, TF_REUSE_SOCKET, 0))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			disconnectEvent_.owner_ = nullptr;
		}
	}
}

void SessionBase::RegisterRecv()
{
	if (bIsConnected_.load() == false)
	{
		return;
	}

	WSABUF wsaBuf;
	DWORD numOfBytes = 0;
	DWORD flags = 0;

	wsaBuf.buf = reinterpret_cast<CHAR*>(recvBuffer_.GetWriteBufferPos());
	wsaBuf.len = recvBuffer_.GetFreeSize();

	recvEvent_.Init();
	recvEvent_.owner_ = shared_from_this();

	if (SOCKET_ERROR == WSARecv(socket_, &wsaBuf, 1, &numOfBytes, &flags, &recvEvent_, nullptr))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			recvEvent_.owner_ = nullptr;
			HandleError(errorCode);
		}
	}
}

void SessionBase::RegisterSend()
{
	sendEvent_.Init();
	sendEvent_.owner_ = shared_from_this();

	{
		WRITE_LOCK;

		int sendSize = 0;
		while (sendQueue_.empty() == false)
		{
			shared_ptr<SendBuffer> sendBuffer = sendQueue_.front();
			sendQueue_.pop();

			sendSize += sendBuffer->GetSize();
			sendEvent_.sendBuffers_.push_back(sendBuffer);
		}
	}

	int numOfBuffers = sendEvent_.sendBuffers_.size();
	vector<WSABUF> wsaBufs(numOfBuffers);
	for (int i = 0; i < numOfBuffers; i++)
	{
		wsaBufs[i].buf = reinterpret_cast<CHAR*>(sendEvent_.sendBuffers_[i]->GetBuffer());
		wsaBufs[i].len = static_cast<ULONG>(sendEvent_.sendBuffers_[i]->GetSize());
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == WSASend(socket_, wsaBufs.data(), numOfBuffers, &numOfBytes, 0, &sendEvent_, nullptr))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent_.owner_ = nullptr;
			sendEvent_.sendBuffers_.clear();
			bIsRegisteredSend_.store(false);
		}
	}
}

void SessionBase::ProcessConnect()
{
	connectEvent_.owner_ = nullptr;
	if (bIsConnected_.exchange(true) == true)
	{
		Disconnect();
		return;
	}

	GetOwnerService()->RegisterSession(GetSessionPtr());

	OnConnected();

	RegisterRecv();
}

void SessionBase::ProcessDisconnect()
{
	disconnectEvent_.owner_ = nullptr;

	GetOwnerService()->DeleteSession(GetSessionPtr());

	OnDisconnect();
}

void SessionBase::ProcessRecv(unsigned int numOfBytes)
{
	recvEvent_.owner_ = nullptr;

	if (numOfBytes == 0)
	{
		Disconnect();
		return;
	}

	if (recvBuffer_.WriteData(numOfBytes) == false)
	{
		// Buffer Overflow
		Disconnect();
		return;
	}

	const int dataSize = recvBuffer_.GetDataSize();
	int processSize = OnRecv(recvBuffer_.GetReadBufferPos(), dataSize);
	if (processSize < 0 || dataSize < processSize || recvBuffer_.ReadData(processSize) == false)
	{
		Disconnect();
		return;
	}

	recvBuffer_.CleanupBuffer();
	RegisterRecv();
}

void SessionBase::ProcessSend(unsigned int numOfBytes)
{
	sendEvent_.owner_ = nullptr;
	sendEvent_.sendBuffers_.clear();

	if (numOfBytes == 0)
	{
		Disconnect();
		return;
	}

	OnSend(numOfBytes);

	{
		WRITE_LOCK;
		if (sendQueue_.empty())
		{
			bIsRegisteredSend_.store(false);
		}
		else
		{
			RegisterSend();
		}
	}
}

void SessionBase::HandleError(int errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect();
		break;
	default:
		// TODO log thread
		cout << "Handle Error: " << errorCode << endl;
		break;
	}
}
