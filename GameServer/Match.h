#pragma once
#include <sqltypes.h>

#include "Scale.h"
#include "Item.h"

#define CHASER_INDEX 0
#define FUGITIVE_FIRST_INDEX 1
#define FUGITIVE_SECOND_INDEX 2
#define FUGITIVE_THIRD_INDEX 3

#define MAX_SCALE_NUMBER 4
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
	using TimePoint = chrono::system_clock::time_point;

	struct MatchRecords
	{
		int playerID;
		bool isChaser;
		bool isFugitive;
		int kills;
		bool isMurdered;
		bool escaped;
		int score;
		SQLLEN len[8]{0,};
	};

	enum : UINT64
	{
		START_TIME_OUT = 1000 * 60 * 1,
		MATCH_END_TICK = 1000 * 60 * 15
	};

	enum
	{
		SCALE_HEIGHT = 15,
		SCALE_WIDTH = 15,
	};

public:
	Match(shared_ptr<Room> owner);
	~Match() override;

	shared_ptr<Match> GetMatchPtr() { return static_pointer_cast<Match>(shared_from_this()); }

public:
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


	// Contents
public:
	void PlayerReadyToReceive(shared_ptr<GameSession> session);
	void PlayerReadyToStart(shared_ptr<GameSession> session);
	void PlayerSetTransform(const shared_ptr<GameSession>& session, int playerIndex, Vector&& position, Rotator&& rotation);
	void PlayerPickUpItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex);
	void PlayerMoveItem(const shared_ptr<GameSession>& session, int playerIndex, int fromIndex, int toIndex, int itemIndex, int targetTopLeftIndex,
	                    bool isRotated);
	void PlayerDropItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex, Vector position,
	                    Rotator rotation);
	void ChaserAttack(const shared_ptr<GameSession>& session, const Vector& position, const Rotator& rotation);
	void HitValidation(const shared_ptr<GameSession>& session, const Vector& position, const Rotator& rotation, int targetPlayerIndex);
	void PlayerDisconnected(const shared_ptr<GameSession>& session);

private:
	void PlayerBackToRoom();

private:
	atomic<bool> isMatchStarted_ = false;
	atomic<bool> isMatchEnded_ = false;
	UINT64 matchStartTick_ = 0;
	UINT64 matchEndTick_ = 0;
	TimePoint matchStartTime_;
	shared_ptr<Room> ownerRoom_;
	// 플레이어 인덱스 0 ~ 3
	vector<shared_ptr<Player>> players_;
	// 저울 인덱스 4 ~ 7
	vector<shared_ptr<Scale>> scales_;
	vector<shared_ptr<Item>> items_;
	wstring matchGUID_;
	wstring matchShortGUID_;
	TimerHandle tickHandle_;
	TimerHandle timeOutHandle_;
};
