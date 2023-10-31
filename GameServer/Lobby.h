#pragma once
#include "Room.h"

class GameService;

class Lobby : public enable_shared_from_this<Lobby>
{
public:
	Lobby(shared_ptr<GameService> service);
	~Lobby();

public:
	shared_ptr<Room> FindRoomByNumber(int roomNumber);
	vector<shared_ptr<Room>> GetRoomList();

	void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	bool CreateRoom(shared_ptr<GameSession> session, string title);
	bool EnterRoom(shared_ptr<GameSession> session, int roomID);
	void LeaveRoom(const shared_ptr<GameSession>& session, int roomNumber);
	void DestroyRoom(int roomNumber);

private:
	USE_LOCK;
	map<int, shared_ptr<Room>> rooms_;
	weak_ptr<GameService> service_;
};
