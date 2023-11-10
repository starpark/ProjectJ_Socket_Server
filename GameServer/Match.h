#pragma once
#include "TickTask.h"
#include "Item.h"

class GameSession;
class Room;
class Scale;

enum class PlayerState
{
	NONE = 0,
	LOADING = 1,
	ALIVE = 2,
	ALIVE_DAMAGED = 3,
	ALIVE_CRITICAL = 4,
	ALIVE_MORIBUND = 5,
	ESCAPED = 6,
	MURDERED = 7,
	DISCONNECTED = 8
};

class Match : public CommandTaskObject
{
	enum : UINT64
	{
		MATCH_END_TICK = 15 * 60 * 1000
	};

public:
	Match(shared_ptr<Room> owner);
	~Match() override;

	void Tick(double deltaTime);
	void Broadcast(shared_ptr<SendBuffer> sendBuffer);

	void Init(shared_ptr<GameSession> chaser, shared_ptr<GameSession> fugitiveFirst,
	          shared_ptr<GameSession> fugitiveSecond,
	          shared_ptr<GameSession> fugitiveThird);
	void Start();
	void End();
	bool CheckEndCondition();
	bool CheckPlayers();
	void PlayerStateChanged(shared_ptr<GameSession> player, PlayerState state);
	void PlayerDisconnected(const shared_ptr<GameSession>& session);

	

public:
	shared_ptr<Match> GetMatchPtr() { return static_pointer_cast<Match>(shared_from_this()); }

public:


private:
	void GenerateItems();
	void PlayerBackToRoom();

private:
	USE_LOCK;
	atomic<bool> matchStated_ = false;
	UINT64 matchEndTick_ = 0;
	shared_ptr<Room> ownerRoom_;
	vector<pair<shared_ptr<Player>, PlayerState>> players_;
	vector<shared_ptr<Scale>> scales_;
	map<int, shared_ptr<Item>> items_;
};
