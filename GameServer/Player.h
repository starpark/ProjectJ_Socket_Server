#pragma once
#include "Inventory.h"

class Match;
class GameSession;

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

	void AddScore(int score)
	{
		score_.fetch_add(score);
	}

	int GetScore() const
	{
		return score_.load();
	}

	ProjectJ::PlayerInfo* GetPlayerInfo();
	ProjectJ::PlayerInitInfo* GetPlayerInitInfo();

private:
	USE_LOCK;
	atomic<ProjectJ::MatchPlayerState> state_;
	atomic<int> score_;
	int sessionID_;
	string nickname_;
	Vector worldPosition_;
	Rotator worldRotation_;
	Vector velocity_;
	weak_ptr<GameSession> ownerSession_;
};
