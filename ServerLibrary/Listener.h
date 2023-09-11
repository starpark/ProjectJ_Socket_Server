#pragma once

#include "Iocp.h"

class Listener : public IocpObject
{
public:
	Listener();
	~Listener();

	// 인터페이스 구현
public:
	HANDLE GetHandle() const override { return reinterpret_cast<HANDLE>(listenSocket_); };
	void ProcessCompletePort(OverlappedEx* overlapped, unsigned numOfBytes) override;

	// 서비스에서 사용
public:
	bool StartListen(shared_ptr<class Service> services);

	// 네트워크 IO 관련
private:
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

private:
	SOCKET listenSocket_;
	// Overlapped 확장 구조체 재사용
	vector<AcceptEvent*> acceptEvents_;
	shared_ptr<class Service> service_;
};
