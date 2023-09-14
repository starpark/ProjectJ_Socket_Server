#pragma once

#include "Iocp.h"
#include "RecvBuffer.h"

#define MAX_BUFFER_SIZE 0x1000

class SessionBase : public IocpObject
{
public:
	SessionBase();
	virtual ~SessionBase();

public: // 인터페이스 구현
	HANDLE GetHandle() const override sealed { return reinterpret_cast<HANDLE>(socket_); };
	void ProcessCompletePort(OverlappedEx* overlapped, unsigned numOfBytes) override;

public: // 외부 사용
	bool Connect();
	void Disconnect();
	void AcceptAndConnect(); // Listener 호출
	void Send(shared_ptr<SendBuffer> sendBuffer);

public: // Getter Setter
	SOCKET GetSocket() const { return socket_; }
	shared_ptr<class Service> GetOwnerService() const { return ownerService_.lock(); }
	BYTE* GetRecvBuffer() { return recvBuffer_.GetWriteBufferPos(); }
	SOCKADDR_IN GetAddress() const { return address_; }
	shared_ptr<SessionBase> GetSessionPtr() { return static_pointer_cast<SessionBase>(shared_from_this()); }
	bool IsConnected() const { return bIsConnected_.load(); }

	void SetOwnerService(shared_ptr<class Service> service) { ownerService_ = service; }
	void SetAddress(SOCKADDR_IN address) { address_ = address; }


protected: // 분산 서버에서 오버라이딩
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
	// 비동기 입출력 관련
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
	SOCKET socket_ = INVALID_SOCKET;
	weak_ptr<class Service> ownerService_;
	SOCKADDR_IN address_;
	atomic<bool> bIsConnected_ = false;
	Lock lock_;

private: // OVERLAPPED 구조체 재사용
	ConnectEvent connectEvent_;
	DisconnectEvent disconnectEvent_;
	RecvEvent recvEvent_;
	SendEvent sendEvent_;

private: // 송신, 수신 관련
	RecvBuffer recvBuffer_;
	queue<shared_ptr<SendBuffer>> sendQueue_;
	atomic<bool> bIsRegisteredSend_;
};
