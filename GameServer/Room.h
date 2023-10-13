#pragma once

class Player;
class GameSession;
	;
class Room
{
	enum
	{
		MAX_PLAYER_NUMBER = 4,
	};
public:
	Room(int roomNumber, string title, const shared_ptr<Player>& hostPlayer);
	~Room();

public:
	int GetRoomNumber() { return roomNumber_; }

	void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	bool ChangePlayerPosition(const shared_ptr<Player>& player, int currentNumber, int desireNumber);
	void PlayerToggleReady(const shared_ptr<Player>& player);

private:
	USE_LOCK;
	int roomNumber_ = -1;
	string title_;
	// 0: Chaser, 1-3: Fugitive
	vector<shared_ptr<Player>> playerSessions_;
	vector<bool> isReady_;
	int hostPlayerNumber = 0;
};

class RoomManager
{
public:
	void CreateRoom(const shared_ptr<Player>& player, string title);
	void EnterRoom(int roomNumber);
	void LeaveRoom(const shared_ptr<Player>& player);
	void DestroyRoom(int roomNumber);

private:
	USE_LOCK;
	unordered_map<int, shared_ptr<Room>> rooms_;
};