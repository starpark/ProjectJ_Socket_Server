#pragma once
#include "NetAddress.h"
#include "Iocp.h"

enum class ServiceType
{
	SERVER = 0,
	CLIENT = 1
};

class SessionBase;
class Listener;

using SessionFactory = function<shared_ptr<SessionBase>(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
	Service() = delete;
	Service(ServiceType type, NetAddress address, SessionFactory factory, uint32_t maxSessionCount = 1);
	virtual ~Service();


public: // 메인 스레드 사용
	virtual bool Init();

public: // 외부 사용
	shared_ptr<SessionBase> CreateSession();
	void RegisterSession(const shared_ptr<SessionBase>& session);
	void DeleteSession(const shared_ptr<SessionBase>& session);
	void Broadcast(shared_ptr<SendBuffer> sendBuffer);

	
public: // Getter
	ServiceType GetServiceType() { return serviceType_; }
	shared_ptr<IocpMain> GetIocpMain() { return iocpMain_; }
	int GetSessionCount() { return sessionCount_; }
	NetAddress GetNetAddress() { return netAddress_; }
	int GetMaxSessionCount() { return maxSessionCount_; }

protected:
	USE_LOCK;
	ServiceType serviceType_;
	int maxSessionCount_ = 0;
	int sessionCount_ = 0;
	NetAddress netAddress_ = {};
	shared_ptr<Listener> listener_;
	shared_ptr<IocpMain> iocpMain_;
	set<shared_ptr<SessionBase>> sessions_;
	SessionFactory sessionFactory_;
};
