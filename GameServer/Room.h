#pragma once

class Player;
class GameSession;
class Lobby;

class Room : enable_shared_from_this<Room>
{
	enum
	{
		MAX_PLAYER_NUMBER = 4,
	};

public:
	Room(int roomNumber, string title, shared_ptr<GameSession> hostSession, shared_ptr<Lobby> lobby);
	~Room();

public:
	int GetID() { return roomID_; }
	int GetNumberOfPlayer() { return numberOfPlayers_; }
	string GetTitle() { return title_; }
	vector<shared_ptr<Player>> GetPlayersInfo();

	bool EnterPlayer(shared_ptr<GameSession> session);
	bool LeavePlayer(const shared_ptr<GameSession>& session);
	void BroadcastHere(shared_ptr<SendBuffer> sendBuffer);
	bool ChangePlayerPosition(const shared_ptr<GameSession>& session, int currentNumber, int desireNumber);
	void ToggleReady(const shared_ptr<GameSession>& session);


private:
	USE_LOCK;
	int roomID_ = -1;
	string title_;
	// 0: Chaser, 1-3: Fugitive
	vector<pair<shared_ptr<GameSession>, bool>> sessionSlots_;
	int hostPlayerNumber_ = 0;
	int numberOfPlayers_ = 0;
	weak_ptr<Lobby> lobby_;
};
