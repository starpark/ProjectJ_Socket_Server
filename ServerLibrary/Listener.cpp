#include "pch.h"
#include "Listener.h"
#include "Service.h"
#include "SessionBase.h"
#include "SocketUtils.h"

Listener::Listener()
	: listenSocket_(INVALID_SOCKET)
{
}

Listener::~Listener()
{
	closesocket(listenSocket_);
	for (AcceptEvent* acceptEvent : acceptEvents_)
	{
		acceptEvent->owner_ = nullptr;
		delete acceptEvent;
	}
	acceptEvents_.clear();
}

void Listener::ProcessCompletePort(OverlappedEx* overlapped, unsigned numOfBytes)
{
	_ASSERT(overlapped->eventType_ == EventType::ACCEPT);
	auto acceptEvent = static_cast<AcceptEvent*>(overlapped);
	ProcessAccept(acceptEvent);
}

bool Listener::StartListen(shared_ptr<Service> service)
{
	service_ = service;
	if (service == nullptr)
	{
		return false;
	}

	listenSocket_ = SocketUtils::CreateSocket();
	if (listenSocket_ == INVALID_SOCKET)
	{
		return false;
	}

	if (service->GetIocpMain()->RegisterHandle(GetHandle()) == false)
	{
		return false;
	}

	if (SocketUtils::SetReuseAddress(listenSocket_, true) == false)
	{
		return false;
	}

	if (SocketUtils::SetLinger(listenSocket_, 0, 0) == false)
	{
		return false;
	}

	/*if (SocketUtils::SetTcpNoDelay(listenSocket_, true) == false)
	{
		return false;
	}*/

	if (SocketUtils::Bind(listenSocket_, service_->GetNetAddress()) == false)
	{
		return false;
	}

	if (SOCKET_ERROR == listen(listenSocket_, SOMAXCONN))
	{
		return false;
	}

	const uint32_t acceptCount = service_->GetMaxSessionCount() / 4;
	for (int i = 0; i < acceptCount; i++)
	{
		auto acceptEvent = new AcceptEvent();
		acceptEvent->owner_ = shared_from_this();
		acceptEvents_.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	_ASSERT(service_ != nullptr);

	shared_ptr<SessionBase> session = service_->CreateSession();
	acceptEvent->Init();
	acceptEvent->session_ = session;

	DWORD numOfBytes = 0;
	if (false == SocketUtils::AcceptEx(listenSocket_, session->GetSocket(), session->GetRecvBuffer(), 0,
	                                   sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &numOfBytes,
	                                   static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	shared_ptr<SessionBase> session = acceptEvent->session_;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), listenSocket_))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	bool nodelayEnable = true;
	if (SOCKET_ERROR == setsockopt(session->GetSocket(), IPPROTO_TCP, TCP_NODELAY,
	                               reinterpret_cast<const char*>(&nodelayEnable), sizeof(nodelayEnable)))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN address;
	int sizeOfAddress = sizeof(address);
	if (SOCKET_ERROR == getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&address), &sizeOfAddress))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetAddress(address);
	session->AcceptAndConnect();

	RegisterAccept(acceptEvent);
}
