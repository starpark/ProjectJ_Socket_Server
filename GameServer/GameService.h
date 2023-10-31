#pragma once
#include "Service.h"
#include "GameSession.h"

class DBConnection;
class Lobby;

class GameService : public Service
{
public:
	GameService(NetAddress address, SessionFactory factory, uint32_t maxSessionCount = 1000);
	~GameService() override;

public:
	bool Init() final;

public:
	shared_ptr<Lobby> GetLobby() { return lobby_; }

	void BroadcastState(SessionState targetState, shared_ptr<SendBuffer> sendBuffer);
	bool ChekcDuplicateConnect(int id);

private:
	set<int> connectedIDs_;
	shared_ptr<Lobby> lobby_;
};
