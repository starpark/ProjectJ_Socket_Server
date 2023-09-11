#pragma once

enum class EventType : __int8
{
	ACCEPT = 0,
	CONNECT,
	DISCONNECT,
	SEND,
	RECV
};

// Overlapped 구조체 확장
class OverlappedEx : public OVERLAPPED
{
public:
	OverlappedEx(EventType type);
	void Init();

public:
	shared_ptr<class IocpObject> owner_; // IO 작업을 요청한 객체
	EventType eventType_;
};

// IO 작업에 따른 이벤트 클래스
// 각 객체는 비동기 IO 호출시 lpOverlapped의 인자로 사용

class AcceptEvent : public OverlappedEx
{
public:
	AcceptEvent() : OverlappedEx(EventType::ACCEPT)
	{
	}

public:
	shared_ptr<class SessionBase> session_;
};

class ConnectEvent : public OverlappedEx
{
public:
	ConnectEvent() : OverlappedEx(EventType::CONNECT)
	{
	}
};

class DisconnectEvent : public OverlappedEx
{
public:
	DisconnectEvent() : OverlappedEx(EventType::DISCONNECT)
	{
	}
};

class SendEvent : public OverlappedEx
{
public:
	SendEvent() : OverlappedEx(EventType::SEND)
	{
	}

public:
	vector<shared_ptr<SendBuffer>> sendBuffers_;
};

class RecvEvent : public OverlappedEx
{
public:
	RecvEvent() : OverlappedEx(EventType::RECV)
	{
	}
};

// Iocp 객체에 등록될 객체 인터페이스
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() const abstract;
	virtual void ProcessCompletePort(OverlappedEx* overlapped, unsigned int numOfBytes) abstract;
};

// Iocp 객체 핸들 랩핑 클래스
class IocpMain
{
public:
	IocpMain();
	~IocpMain();

	bool RegisterHandle(HANDLE handle);
	void WorkThread(unsigned int timeout = INFINITE);
	HANDLE GetHandle() const { return iocpHandle_; }

private:
	HANDLE iocpHandle_;
};
