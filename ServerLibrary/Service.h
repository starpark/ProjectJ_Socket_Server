#pragma once
#include "NetAddress.h"
#include "Iocp.h"

enum class ServiceType
{
	SERVER = 0,
	CLIENT = 1
};

class SessionBase;

using SessionFactory = function<shared_ptr<SessionBase>(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
	Service() = delete;
	Service(ServiceType type, NetAddress address, SessionFactory factory, uint32_t maxSessionCount = 1);
	virtual ~Service();


public: // 메인 스레드 사용
	virtual bool Init();
	virtual void Start() abstract;

public: // 외부 사용
	shared_ptr<SessionBase> CreateSession();
	void RegisterSession(shared_ptr<SessionBase> session);
	void DeleteSession(shared_ptr<SessionBase> session);
	void BroadCastWithoutSelf(shared_ptr<SessionBase> self, shared_ptr<SendBuffer> sendBuffer);


public: // Getter
	ServiceType GetServiceType() { return serviceType_; }
	shared_ptr<IocpMain> GetIocpMain() { return iocpMain_; }
	unsigned int GetSessionCount() { return sessionCount_; }
	NetAddress GetNetAddress() { return netAddress_; }
	uint32_t GetMaxSessionCount() { return maxSessionCount_; }

protected:
	USE_LOCK;
	ServiceType serviceType_;
	NetAddress netAddress_ = {};
	SessionFactory sessionFactory_; // Session 생성 함수
	uint32_t maxSessionCount_ = 0;
	shared_ptr<class Listener> listener_; // listen 담당 객체

	shared_ptr<IocpMain> iocpMain_; // IOCP 객체

	set<shared_ptr<SessionBase>> sessions_; // 연결된 Session
	uint32_t sessionCount_ = 0;
};
