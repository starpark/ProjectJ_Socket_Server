#pragma once
#include "Room.h"

class GameService;

class Lobby : public CommandTaskObject
{
public:
	Lobby(shared_ptr<GameService> service);
	~Lobby() override;

public:
	shared_ptr<Room> FindRoomByNumber(int roomNumber);
	vector<shared_ptr<Room>> GetRoomList();
	shared_ptr<Lobby> GetLobbyPtr() { return static_pointer_cast<Lobby>(shared_from_this()); }

	void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	shared_ptr<Room> CreateRoom(shared_ptr<GameSession> session, string title);
	shared_ptr<Room> FindRoom(int roomID);
	void DestroyRoom(int roomNumber);

private:
	map<int, shared_ptr<Room>> rooms_;
	weak_ptr<GameService> service_;
};
