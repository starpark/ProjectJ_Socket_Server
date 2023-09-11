#include "pch.h"
#include "GameService.h"

#include "DBConnection.h"

GameService::GameService(NetAddress address, SessionFactory factory, uint32_t maxSessionCount)
	: Service(ServiceType::SERVER, address, factory, maxSessionCount)
{
}

GameService::~GameService()
{
}

bool GameService::Init()
{
	if (Service::Init() == false)
	{
		cout << "Failed to Service Init" << endl;
		return false;
	}

	return true;
}

void GameService::Start()
{
	while (true)
	{
		iocpMain_->WorkThread(10);

		// TODO
	}
}
