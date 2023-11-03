#pragma once
#include "TickTask.h"
#include "Item.h"

class GameSession;
class Room;

class Match : public TickTask
{
public:
	Match(shared_ptr<GameSession> chaser, shared_ptr<GameSession> fugitiveOne, shared_ptr<GameSession> fugitiveTwo,
	      shared_ptr<GameSession> fugitiveThree, shared_ptr<Room> owner);
	~Match() override;

	void Tick(double deltaTime) override;

	void MatchInit();
	void MatchStart();
	void MatchEnd();

	void PlayerDisconnected(const shared_ptr<Player>& disconnectedPlayer);
	void BroadcastMatchInfo();

private:
	USE_LOCK;
	// first player ptr, second is connected
	vector<pair<shared_ptr<Player>, bool>> players_;
	shared_ptr<ItemManager> itemManager_;
	shared_ptr<Room> ownerRoom_;
};
