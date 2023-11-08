#include "pch.h"
#include "Lobby.h"
#include "GameService.h"
#include "Message.pb.h"

Lobby::Lobby(shared_ptr<GameService> service)
	: service_(service)
{
	service_ = service;
}

Lobby::~Lobby()
{
	rooms_.clear();
}

shared_ptr<Room> Lobby::FindRoomByNumber(int roomNumber)
{
	if (rooms_.find(roomNumber) != rooms_.end())
	{
		return rooms_[roomNumber];
	}

	return nullptr;
}

vector<shared_ptr<Room>> Lobby::GetRoomList()
{
	vector<shared_ptr<Room>> list;
	for (auto room : rooms_)
	{
		list.push_back(room.second);
	}

	return list;
}

void Lobby::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	if (auto service = service_.lock())
	{
		service->BroadcastState(SessionState::LOBBY, sendBuffer);
	}
}

shared_ptr<Room> Lobby::CreateRoom(shared_ptr<GameSession> session, string title)
{
	static atomic<int> roomIDGenerator = 1;
	int roomID = roomIDGenerator.fetch_add(1);

	if (title.size() > 60)
	{
		title = title.substr(0, 60);
	}

	shared_ptr<Room> newRoom = ObjectPool<Room>::MakeShared(roomID, title, session, GetLobbyPtr());
	//auto newRoom = make_shared<Room>(roomID, title.substr(0, 20), session, GetLobbyPtr());

	GLogHelper->Reserve(LogCategory::Log_INFO, "Lobby Create New Room Title:%s ID: %d\n", title.c_str(), roomID);

	rooms_.insert({roomID, newRoom});

	return newRoom;
}

shared_ptr<Room> Lobby::FindRoom(shared_ptr<GameSession> session, int roomID)
{
	if (auto room = FindRoomByNumber(roomID))
	{
		return room;
	}

	return nullptr;
}


void Lobby::DestroyRoom(int roomNumber)
{
	auto roomIter = rooms_.find(roomNumber);
	if (roomIter != rooms_.end())
	{
		GLogHelper->Reserve(LogCategory::Log_INFO, L"Lobby Destroy Room %d\n", roomNumber);
		shared_ptr<Room> room = roomIter->second;

		rooms_.erase(roomNumber);
	}
}
