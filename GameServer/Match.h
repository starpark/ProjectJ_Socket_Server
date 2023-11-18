#pragma once
#include "Scale.h"
#include "Item.h"

#define CHASER_INDEX 0
#define FUGITIVE_FIRST_INDEX 1
#define FUGITIVE_SECOND_INDEX 2
#define FUGITIVE_THIRD_INDEX 3

#define SCALE_INDEX_PREFIX 4
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
		START_TIME_OUT = 1000 * 60 * 1,
		MATCH_END_TICK = 1000 * 60 * 15
	};

public:
	Match(shared_ptr<Room> owner);
	~Match() override;

	shared_ptr<Match> GetMatchPtr() { return static_pointer_cast<Match>(shared_from_this()); }
	ProjectJ::PlayerInfo* GetPlayerInfo(int playerIndex);

	void Tick();
	void Broadcast(shared_ptr<SendBuffer> sendBuffer);

	void Init(shared_ptr<GameSession> chaser,
	          shared_ptr<GameSession> fugitiveFirst,
	          shared_ptr<GameSession> fugitiveSecond,
	          shared_ptr<GameSession> fugitiveThird);
	void Start();
	void End();
	void StartTimeOut();
	bool CheckPlayersState();
	void PlayerStateChanged(const shared_ptr<GameSession>& session, ProjectJ::MatchPlayerState state);
	void PlayerDisconnected(const shared_ptr<GameSession>& session);

	// Contents
public:
	void PlayerReadyToReceive(shared_ptr<GameSession> session);
	void PlayerReadyToStart(shared_ptr<GameSession> session);
	void PlayerPickUpItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex);
	void PlayerMoveItem(const shared_ptr<GameSession>& session, int playerIndex, int fromIndex, int toIndex, int itemIndex, int targetTopLeftIndex,
	                    bool isRotated);
	void PlayerDropItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex, Vector position,
	                    Rotator rotation);
	void ChaserAttack(const shared_ptr<GameSession>& session, Vector position, Rotator rotation);
	void HitValidation(const shared_ptr<GameSession>& session, Vector position, Rotator rotation, int targetPlayerIndex);

private:
	void PlayerBackToRoom();

private:
	USE_LOCK;
	atomic<bool> isMatchStarted_ = false;
	atomic<bool> isMatchEnded_ = false;
	UINT64 matchEndTick_ = 0;
	shared_ptr<Room> ownerRoom_;
	// 플레이어 인덱스 0 ~ 3
	vector<pair<shared_ptr<Player>, ProjectJ::MatchPlayerState>> players_;
	// 저울 인덱스 4 ~ 7
	vector<shared_ptr<Scale>> scales_;
	vector<shared_ptr<Item>> items_;
	TimerHandle tickHandle_;
	TimerHandle timeOutHandle_;
};
