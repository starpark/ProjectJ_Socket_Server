#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#include "Player.h"
#include "Lobby.h"

Room::Room(int roomNumber, string title, shared_ptr<GameSession> hostSession, shared_ptr<Lobby> lobby)
	: roomID_(roomNumber), title_(move(title)), lobby_(lobby)
{
	sessionSlots_.assign(4, make_pair(nullptr, false));

	sessionSlots_[0].first = hostSession;
}

Room::~Room()
{
	sessionSlots_.clear();
}

vector<shared_ptr<Player>> Room::GetPlayersInfo()
{
	vector<shared_ptr<Player>> players;

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		players.push_back(sessionSlots_[i].first->GetPlayer());
	}

	return players;
}

bool Room::EnterPlayer(shared_ptr<GameSession> session)
{
	WRITE_LOCK;
	if (numberOfPlayers_ == MAX_PLAYER_NUMBER)
	{
		return false;
	}

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == nullptr)
		{
			// TODO Broadcast

			sessionSlots_[i].first = session;
			sessionSlots_[i].second = false;

			return true;
		}
	}

	return false;
}

bool Room::LeavePlayer(const shared_ptr<GameSession>& session)
{
	WRITE_LOCK;
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == session)
		{
			// TODO Broadcast

			sessionSlots_[i].first = nullptr;
			sessionSlots_[i].second = false;

			return true;
		}
	}

	return false;
}

void Room::BroadcastHere(shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;
	for (auto session : sessionSlots_)
	{
		session.first->Send(sendBuffer);
	}
}

bool Room::ChangePlayerPosition(const shared_ptr<GameSession>& session, int currentNumber, int desireNumber)
{
	if (currentNumber < 0 || currentNumber > 4 || desireNumber < 0 || desireNumber > 4)
	{
		return false;
	}

	WRITE_LOCK;
	if (sessionSlots_[desireNumber].first == nullptr && sessionSlots_[currentNumber].first == session)
	{
		swap(sessionSlots_[desireNumber].first, sessionSlots_[currentNumber].first);
		//TODO Broadcast
	}

	return true;
}

void Room::ToggleReady(const shared_ptr<GameSession>& session)
{
	WRITE_LOCK;
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == session)
		{
			sessionSlots_[i].second ^= true;
		}
	}
}
