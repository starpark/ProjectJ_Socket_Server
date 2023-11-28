#pragma once
#include "Inventory.h"
#include "DataManager.h"

class Match;
class GameSession;

constexpr float PLAYER_DEFAULT_MOVE_SPEED = 400.0f;
constexpr float PLAYER_ADRENALINE_MOVE_SPEED = 480.0f;
constexpr UINT64 ADRENALINE_DURATION_TICK = 3000;
constexpr float PLAYER_MAX_MOVE_SPEED_SLOW_RATIO = 0.25f;

class Player : public Inventory
{
public:
	Player(int index, int row, int column, int maxWeight, int sessionID, string nickname);
	~Player() override;

	shared_ptr<GameSession> GetOwnerSession() { return ownerSession_.lock(); }
	int GetID() const { return sessionID_; }
	string GetNickname() const { return nickname_; }
	ProjectJ::MatchPlayerState GetState() const { return state_.load(); }

	Vector GetVector()
	{
		READ_LOCK;
		return worldPosition_;
	}

	Rotator GetRotation()
	{
		READ_LOCK;
		return worldRotation_;
	}

	float GetMoveSpeed()
	{
		READ_LOCK;
		return moveSpeed_;
	}

	int GetAcquiredItemCount()
	{
		READ_LOCK;
		return acquireItemCount_;
	}

	int GetFugitiveHitCount()
	{
		READ_LOCK;
		return fugitiveHitCount_;
	}

	int GetChaserKillCount()
	{
		READ_LOCK;
		return chaserKillCount_;
	}

	int GetChaserHitCount()
	{
		READ_LOCK;
		return chaserHitCount_;
	}

	void SetSession(const shared_ptr<GameSession>& session)
	{
		WRITE_LOCK;
		ownerSession_ = session;
	}

	void SetPosition(const Vector& position)
	{
		WRITE_LOCK;
		worldPosition_ = position;
	}

	void SetPosition(Vector&& position)
	{
		WRITE_LOCK;
		worldPosition_ = move(position);
	}

	void SetRotation(const Rotator& rotation)
	{
		WRITE_LOCK;
		worldRotation_ = rotation;
	}

	void SetRotatoin(Rotator&& rotation)
	{
		WRITE_LOCK;
		worldRotation_ = move(rotation);
	}

	void SetTransform(const Vector& position, const Rotator& rotation)
	{
		WRITE_LOCK;
		worldPosition_ = position;
		worldRotation_ = rotation;
	}

	void SetTransform(Vector&& position, Rotator&& rotation, Vector&& velocity)
	{
		WRITE_LOCK;

		worldPosition_ = move(position);
		worldRotation_ = move(rotation);
		velocity_ = move(velocity);
	}

	void SetState(ProjectJ::MatchPlayerState state)
	{
		state_.store(state);
	}

	void SetMoveSpeed(float speed)
	{
		WRITE_LOCK;
		moveSpeed_ = speed;
	}

	void AddScore(int score)
	{
		score_.fetch_add(score);
		AddAcquireItemCount();
	}

	void AddAcquireItemCount()
	{
		WRITE_LOCK;
		++acquireItemCount_;
	}

	void ActiveAdrenaline(UINT64 endTick)
	{
		WRITE_LOCK;
		isAdrenaline_ = true;
		adrenalineEndTick_ = endTick;
	}

	void AddFugitiveHitCount()
	{
		READ_LOCK;
		++fugitiveHitCount_;
	}

	void AddChaserKillCount()
	{
		WRITE_LOCK;
		++chaserKillCount_;
	}

	void AddChaserHitCount()
	{
		WRITE_LOCK;
		++chaserHitCount_;
	}

	void CheckAdrenalineEnd(UINT64 currentTick)
	{
		WRITE_LOCK;
		if (isAdrenaline_ == true)
		{
			if (currentTick >= adrenalineEndTick_)
			{
				isAdrenaline_ = false;
			}
		}
	}

	int GetScore() const
	{
		return score_.load();
	}

	float GetCurrentMoveSpeed()
	{
		READ_LOCK;
		if (isAdrenaline_ == true)
		{
			return PLAYER_ADRENALINE_MOVE_SPEED;
		}
		return PLAYER_DEFAULT_MOVE_SPEED - (PLAYER_DEFAULT_MOVE_SPEED * (static_cast<float>(GetCurrentWeight()) / static_cast<float>(GetMaxWeight()) *
			PLAYER_MAX_MOVE_SPEED_SLOW_RATIO));
	}

	ProjectJ::PlayerInfo* GetPlayerInfo();
	ProjectJ::PlayerInitInfo* GetPlayerInitInfo();
	InventoryErrorCode TryAddItem(const shared_ptr<Item>& item) final;
	InventoryErrorCode RelocateItem(const shared_ptr<Inventory>& to, const shared_ptr<Item>& item, int slotIndex, bool isRotated) final;
	InventoryErrorCode DropItem(const shared_ptr<Item>& item, Vector position, Rotator rotation) final;


private:
	USE_LOCK;
	atomic<ProjectJ::MatchPlayerState> state_;
	atomic<int> score_;
	int sessionID_;
	string nickname_;
	Vector worldPosition_;
	Rotator worldRotation_;
	Vector velocity_;
	float moveSpeed_;
	bool isAdrenaline_ = false;
	UINT64 adrenalineEndTick_ = 0;
	int acquireItemCount_ = 0;
	int fugitiveHitCount_ = 0;
	int chaserKillCount_ = 0;
	int chaserHitCount_ = 0;
	weak_ptr<GameSession> ownerSession_;
};
