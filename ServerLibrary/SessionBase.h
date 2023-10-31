#pragma once

#include "Iocp.h"
#include "RecvBuffer.h"

struct PacketHeader
{
	uint16_t size;
	uint16_t type;
};

class SessionBase : public IocpObject
{
public:
	SessionBase();
	virtual ~SessionBase();

public:
	HANDLE GetHandle() const override sealed { return reinterpret_cast<HANDLE>(socket_); };
	void ProcessCompletePort(OverlappedEx* overlapped, unsigned numOfBytes) override;

public:
	bool Connect();
	bool IsConnected() const { return bIsConnected_.load(); }
	void Disconnect();
	void AcceptAndConnect();
	void Send(shared_ptr<SendBuffer> sendBuffer);

public: // Getter Setter
	SOCKET GetSocket() const { return socket_; }
	shared_ptr<class Service> GetService() const { return service_.lock(); }
	BYTE* GetRecvBuffer() { return recvBuffer_.GetWriteBufferPos(); }
	SOCKADDR_IN GetAddress() const { return address_; }
	shared_ptr<SessionBase> GetSessionPtr() { return static_pointer_cast<SessionBase>(shared_from_this()); }

	void SetService(shared_ptr<Service> service) { service_ = service; }
	void SetAddress(SOCKADDR_IN address) { address_ = address; }


protected:
	virtual void OnConnected()
	{
	}

	virtual void OnDisconnect()
	{
	}

	virtual int OnRecv(BYTE* buffer, int numOfBytes) { return numOfBytes; }

	virtual void OnSend(int numOfBytes)
	{
	}

private:
	bool RegisterConnect();
	void RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(unsigned int numOfBytes);
	void ProcessSend(unsigned int numOfBytes);

	void HandleError(int errorCode);

protected:
	USE_LOCK;
	SOCKET socket_ = INVALID_SOCKET;
	weak_ptr<Service> service_;
	SOCKADDR_IN address_;
	atomic<bool> bIsConnected_ = false;

private:
	ConnectEvent connectEvent_;
	DisconnectEvent disconnectEvent_;
	RecvEvent recvEvent_;
	SendEvent sendEvent_;

private:
	RecvBuffer recvBuffer_;
	queue<shared_ptr<SendBuffer>> sendQueue_;
	atomic<bool> bIsRegisteredSend_;
};
