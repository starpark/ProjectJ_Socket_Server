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
	Vector GetVector() const { return worldPosition_; }
	Rotator GetRotator() const { return worldRotation_; }

	void SetSession(const shared_ptr<GameSession>& session) { ownerSession_ = session; }
	void SetInfo(ProjectJ::Vector position, ProjectJ::Rotator rotation);

private:
	int sessionID_;
	string nickname_;
	Vector worldPosition_;
	Rotator worldRotation_;
	weak_ptr<GameSession> ownerSession_;
};
