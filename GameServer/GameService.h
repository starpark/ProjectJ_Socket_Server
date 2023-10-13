#pragma once

#include "Service.h"

class DBConnection;

class GameService : public Service
{
public:
	GameService(NetAddress address, SessionFactory factory, uint32_t maxSessionCount = 1000);
	~GameService() override;

public:
	bool Init() final;
};
