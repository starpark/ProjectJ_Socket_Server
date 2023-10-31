#include "pch.h"
#include "GameService.h"
#include "Lobby.h"

GameService::GameService(NetAddress address, SessionFactory factory, uint32_t maxSessionCount)
	: Service(ServiceType::SERVER, address, factory, maxSessionCount)
{
}

GameService::~GameService()
{
}


void GameService::BroadcastState(SessionState targetState, shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;
	for (auto session : sessions_)
	{
		shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
		if (gameSession->GetState() == targetState)
		{
			session->Send(sendBuffer);
		}
	}
}

bool GameService::Init()
{
	if (Service::Init() == false)
	{
		cout << "Failed to Service Init" << endl;
		return false;
	}

	auto thisService = static_pointer_cast<GameService>(shared_from_this());
	lobby_ = make_shared<Lobby>(thisService);

	return true;
}
