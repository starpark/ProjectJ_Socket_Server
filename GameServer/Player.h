#pragma once
#include "Inventory.h"

class Match;
class GameSession;

class Player : public Inventory
{
public:
	Player(int index, int row, int column, int maxWeight, const shared_ptr<Match>& match);
	virtual ~Player();

	shared_ptr<GameSession> GetOwnerSession() { return ownerSession_.lock(); }
	int GetIndex() const { return index_; }
	Vector GetVector() const { return worldPosition_; }
	Rotator GetRotator() const { return worldRotation_; }

	void SetSession(const shared_ptr<GameSession>& session) { ownerSession_ = session; }
	void SetInfo(ProjectJ::Vector position, ProjectJ::Rotator rotation);
	void ResetMatch() { match_.reset(); }

private:
	int index_;
	Vector worldPosition_;
	Rotator worldRotation_;
	shared_ptr<Match> match_;
	weak_ptr<GameSession> ownerSession_;
};
