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
		MATCH_END_TICK = 1000 * 60 * 5
	};

	enum
	{
		INV_WEIGHT_LIMIT = 1000,
		SCALE_INV_WEIGHT_CRITERIA = 2000,
		SCALE_INV_WEIGHT_TOLERANCE = 50,
		CHASER_INV_ROW = 5,
		CHASER_INV_COL = 5,
		FUGITIVE_INV_ROW = 7,
		FUGITIVE_INV_COL = 5,
		SCALE_INV_ROW = 10,
		SCALE_INV_COL = 10,
	};

public:
	enum : int
	{
		ESCAPE_SCORE = 1000,
		PICKUP_NEW_ITEM_SCORE = 10,
		CHASER_HIT_SUCCESS_SCORE = 300,
		FUGITIVE_HIT_SCORE = 50,
		CHASER_KILL_SCORE = 500,
		ACTIVATING_ESCAPE_SCORE = 100,
		ACTIVATING_ESCAPE_CONTRIBUTION_SCORE = 100,
	};

	enum : int
	{
		DEFAULT_WALK_SPEED = 400
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


	// Contents
public:
	void PlayerReadyToReceive(shared_ptr<GameSession> session);
	void PlayerSetOwnSpawnLocation(const shared_ptr<GameSession>& session, int playerIndex, Vector&& position, Rotator&& rotation);
	void PlayerReadyToStart(shared_ptr<GameSession> session, int playerIndex);
	void PlayerSetTransform(const shared_ptr<GameSession>& session, int playerIndex, Vector&& position, Rotator&& rotation, Vector&& velocity);
	void PlayerPickUpItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex);
	void PlayerMoveItem(const shared_ptr<GameSession>& session, int playerIndex, int fromIndex, int toIndex, int itemIndex, int targetTopLeftIndex,
	                    bool isRotated);
	void PlayerDropItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex, Vector position,
	                    Rotator rotation);
	void ChaserAttack(const shared_ptr<GameSession>& session, const Vector& position, const Rotator& rotation);
	void HitValidation(const shared_ptr<GameSession>& session, const Vector& position, const Rotator& rotation, int targetPlayerIndex);
	void FugitiveEscape(const shared_ptr<GameSession>& session, int playerIndex, int scaleIndex);
	void ChaserInstallCCTV(const shared_ptr<GameSession>& session, const Vector& position, const Rotator& rotation);
	void PlayerDisconnected(const shared_ptr<GameSession>& session);
	void PlayerLeaveMatch(const shared_ptr<GameSession>& session, int playerIndex);

private:
	int CalulatePlayerScore(int playerIndex);
	void PlayerBackToRoom();

	bool IsPlayer(int index)
	{
		return CHASER_INDEX <= index && index <= FUGITIVE_THIRD_INDEX;
	}

	bool IsChaser(int index)
	{
		return index == CHASER_INDEX;
	}

	bool IsFugitive(int index)
	{
		return FUGITIVE_FIRST_INDEX <= index && index <= FUGITIVE_THIRD_INDEX;
	}

	bool IsScale(int index)
	{
		return index >= MAX_SCALE_NUMBER && index < MAX_INVENTORY_INDEX;
	}

	bool IsValidInventoryIndex(int index)
	{
		return IsPlayer(index) || IsScale(index);
	}

	bool IsValidItemIndex(int index)
	{
		return 0 <= index && index < items_.size();
	}

	bool IsPlayerAlive(ProjectJ::MatchPlayerState state)
	{
		if (state != ProjectJ::ALIVE && state != ProjectJ::ALIVE_DAMAGED && state != ProjectJ::ALIVE_CRITICAL &&
			state != ProjectJ::ALIVE_MORIBUND)
		{
			return false;
		}

		return true;
	}


private:
	atomic<bool> isMatchInitialized_ = false;
	atomic<bool> isMatchStarted_ = false;
	atomic<bool> isMatchEnded_ = false;
	UINT64 matchStartTick_ = 0;
	UINT64 matchEndTick_ = 0;
	atomic<UINT64> matchCurrentTick_ = 0;
	TimePoint matchStartTime_;
	shared_ptr<Room> ownerRoom_;
	// 플레이어 인덱스 0 ~ 3
	vector<shared_ptr<Player>> players_;
	// 저울 인덱스 4 ~ 7
	vector<shared_ptr<Scale>> scales_;
	vector<shared_ptr<Item>> items_;
	vector<Vector> installedCCTV_;
	atomic<int> remainingCCTV_;
	wstring matchGUID_;
	wstring matchShortGUID_;
	TimerHandle tickHandle_;
	TimerHandle timeOutHandle_;
};
