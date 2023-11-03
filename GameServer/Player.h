#pragma once
#include "Inventory.h"

class Match;
class GameSession;

struct Vector
{
	float x;
	float y;
	float z;
};

struct Rotator
{
	float roll;
	float pitch;
	float yaw;
};

class Player
{
public:
	Player(const shared_ptr<Match>& match);
	~Player();

	shared_ptr<GameSession> GetOwnerSession() { return ownerSession_.lock(); }

	void SetSession(const shared_ptr<GameSession>& session) { ownerSession_ = session; }
	void SetInfo(ProjectJ::Vector position, ProjectJ::Rotator rotation);

private:
	USE_LOCK;
	Vector worldPosition_;
	Rotator worldRotation_;
	shared_ptr<Match> match_;
	shared_ptr<Inventory> inventory_;
	weak_ptr<GameSession> ownerSession_;
};
