#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#include "Player.h"

Room::Room(int roomNumber, string title, const shared_ptr<Player>& hostPlayer)
	:roomNumber_(roomNumber), title_(move(title))
{
	playerSessions_.assign(4, nullptr);
	isReady_.assign(4, false);

	playerSessions_[0] = hostPlayer;
}

Room::~Room()
{
}

void Room::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	for(auto player : playerSessions_)
	{
		if (shared_ptr<GameSession> session = player->ownerSession_.lock())
		{
			session->Send(sendBuffer);
		}
	}
}

bool Room::ChangePlayerPosition(const shared_ptr<Player>& player, int currentNumber, int desireNumber)
{
	if(currentNumber < 0 || currentNumber > 4 || desireNumber < 0 || desireNumber > 4)
	{
		return false;
	}

	WRITE_LOCK;
	if(playerSessions_[desireNumber] == nullptr && playerSessions_[currentNumber] == player)
	{
		playerSessions_[desireNumber] = player;
		playerSessions_[currentNumber] = nullptr;
	}

	return true;
}

void Room::PlayerToggleReady(const shared_ptr<Player>& player)
{
	WRITE_LOCK;
	for(int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if(playerSessions_[i] == player)
		{
			isReady_[i] = 1 ^ isReady_[i];
		}
	}
}

void RoomManager::CreateRoom(const shared_ptr<Player>& player, string title)
{
	static atomic<int> roomNumberGenerator = 1;
	int roomNumber = roomNumberGenerator.fetch_add(1);
	shared_ptr<Room> newRoom = make_shared<Room>(roomNumber, title.substr(0, 20), player);

	WRITE_LOCK;
	rooms_.insert({ roomNumber , newRoom });
}

void RoomManager::EnterRoom(int roomNumber)
{
}

void RoomManager::LeaveRoom(const shared_ptr<Player>& player)
{
}

void RoomManager::DestroyRoom(int roomNumber)
{
}
