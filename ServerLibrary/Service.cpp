#include "pch.h"
#include "Service.h"
#include "SessionBase.h"
#include "Listener.h"
#include "SocketUtils.h"

Service::Service(ServiceType type, NetAddress address, SessionFactory factory, uint32_t maxSessionCount)
	: serviceType_(type), netAddress_(address), sessionFactory_(factory), maxSessionCount_(maxSessionCount),
	  listener_(nullptr), iocpMain_(make_shared<IocpMain>())
{
}

Service::~Service()
{
	iocpMain_ = nullptr;
	listener_ = nullptr;
}

bool Service::Init()
{
	if (sessionFactory_ == nullptr)
	{
		return false;
	}

	listener_ = make_shared<Listener>();
	if (listener_ == nullptr)
	{
		return false;
	}

	if (listener_->StartListen(shared_from_this()) == false)
	{
		return false;
	}

	return true;
}

shared_ptr<SessionBase> Service::CreateSession()
{
	shared_ptr<class SessionBase> session = sessionFactory_();
	session->SetOwnerService(shared_from_this());

	if (iocpMain_->RegisterHandle(session->GetHandle()) == false)
	{
		return nullptr;
	}

	return session;
}

void Service::RegisterSession(shared_ptr<SessionBase> session)
{
	LockGuard gurad(lock_);
	sessionCount_++;
	sessions_.insert(session);
}

void Service::DeleteSession(shared_ptr<SessionBase> session)
{
	LockGuard gurad(lock_);
	sessions_.erase(session);
	sessionCount_--;
}


void Service::BroadCastWithoutSelf(shared_ptr<SessionBase> self, shared_ptr<SendBuffer> sendBuffer)
{
	LockGuard gurad(lock_);
	for (auto session : sessions_)
	{
		if (session == self) continue;
		session->Send(sendBuffer);
	}
}
