#include "pch.h"
#include "Lobby.h"
#include "GameService.h"

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
	READ_LOCK;
	if (rooms_.find(roomNumber) != rooms_.end())
	{
		return rooms_[roomNumber];
	}

	return nullptr;
}

vector<shared_ptr<Room>> Lobby::GetRoomList()
{
	READ_LOCK;
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

bool Lobby::CreateRoom(shared_ptr<GameSession> session, string title)
{
	static atomic<int> roomIDGenerator = 1;
	int roomID = roomIDGenerator.fetch_add(1);
	auto newRoom = make_shared<Room>(roomID, title.substr(0, 20), session, shared_from_this());

	WRITE_LOCK;
	auto [iterator, result] = rooms_.insert({roomID, newRoom});
	return result;
}

bool Lobby::EnterRoom(shared_ptr<GameSession> session, int roomID)
{
	WRITE_LOCK;
	if (auto room = FindRoomByNumber(roomID))
	{
		if (room->EnterPlayer(session))
		{
			session->SetState(SessionState::ROOM);
			return true;
		}
	}

	return false;
}

void Lobby::LeaveRoom(const shared_ptr<GameSession>& session, int roomNumber)
{
	WRITE_LOCK;
	if (auto room = FindRoomByNumber(roomNumber))
	{
		if (room->LeavePlayer(session))
		{
			session->SetState(SessionState::LOBBY);
			if (room->GetNumberOfPlayer() == 0)
			{
				DestroyRoom(roomNumber);
			}
		}
	}
}

void Lobby::DestroyRoom(int roomNumber)
{
	WRITE_LOCK;
	if (rooms_.find(roomNumber) != rooms_.end())
	{
		rooms_.erase(roomNumber);
	}
}
