#include "pch.h"
#include "Service.h"
#include "SessionBase.h"
#include "Listener.h"
#include "SocketUtils.h"

Service::Service(ServiceType type, NetAddress address, SessionFactory factory, uint32_t maxSessionCount)
	: serviceType_(type), maxSessionCount_(maxSessionCount), netAddress_(address), listener_(nullptr),
	  iocpMain_(make_shared<IocpMain>()), sessionFactory_(factory)
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
	shared_ptr<SessionBase> session = sessionFactory_();
	session->SetService(shared_from_this());

	if (iocpMain_->RegisterHandle(session->GetHandle()) == false)
	{
		return nullptr;
	}

	return session;
}

void Service::RegisterSession(const shared_ptr<SessionBase>& session)
{
	WRITE_LOCK;
	sessionCount_++;
	sessions_.insert(session);
}

void Service::DeleteSession(const shared_ptr<SessionBase>& session)
{
	WRITE_LOCK;
	sessions_.erase(session);
	sessionCount_--;
}


void Service::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;
	for (auto session : sessions_)
	{
		session->Send(sendBuffer);
	}
}
