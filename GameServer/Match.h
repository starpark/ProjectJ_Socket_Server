#pragma once
#include "Scale.h"
#include "Item.h"

#define CHASER_INDEX 0
#define FUGITIVE_FIRST_INDEX 1
#define FUGITIVE_SECOND_INDEX 2
#define FUGITIVE_THIRD_INDEX 3

#define SCALE_FIRST_INDEX 0
#define SCALE_SECOND_INDEX 1
#define SCALE_THIRD_INDEX 2
#define SCALE_FOURTH_INDEX 3

#define MAX_INVENTORY_INDEX 8

class GameSession;
class Room;
class Scale;

class Match : public CommandTaskObject
{
	enum : UINT64
	{
		MATCH_END_TICK = 1000 * 60 * 15
	};

	// State
public:
	Match(shared_ptr<Room> owner);
	~Match() override;

	shared_ptr<Match> GetMatchPtr() { return static_pointer_cast<Match>(shared_from_this()); }
	ProjectJ::PlayerInfo* GetPlayerInfo(int playerIndex);

	void Tick(double deltaTime);
	void Broadcast(shared_ptr<SendBuffer> sendBuffer);

	void Init(shared_ptr<GameSession> chaser,
	          shared_ptr<GameSession> fugitiveFirst,
	          shared_ptr<GameSession> fugitiveSecond,
	          shared_ptr<GameSession> fugitiveThird);
	void Start();
	void End();
	bool CheckEndCondition();
	bool CheckPlayersState();
	void PlayerStateChanged(const shared_ptr<GameSession>& session, ProjectJ::MatchPlayerState state);
	void PlayerDisconnected(const shared_ptr<GameSession>& session);

	// Contents
public:
	void PlayerLoadingComplete(shared_ptr<GameSession> session);
	void PlayerPickupItem(int playerIndex, int itemIndex);
	void PlayerMoveItem(int playerIndex, int fromIndex, int toIndex, int itemIndex, int targetTopLeftIndex, bool isRotated);
	void PlayerDropItem(int playerIndex, int itemIndex, ProjectJ::Vector position, ProjectJ::Rotator rotation);

public:
private:
	void GenerateItems();
	void PlayerBackToRoom();

private:
	USE_LOCK;
	atomic<bool> matchStated_ = false;
	UINT64 matchEndTick_ = 0;
	shared_ptr<Room> ownerRoom_;
	vector<pair<shared_ptr<Player>, ProjectJ::MatchPlayerState>> players_;
	vector<shared_ptr<Scale>> scales_;
	vector<shared_ptr<Item>> items_;
};
