#pragma once

class Player
{
public:
	int playerId_ = -1;
	string nickname_;
	string name_;
	weak_ptr<class GameSession> ownerSession_;
};

