#include "pch.h"
#include "Room.h"

#include "GamePacketHandler.h"
#include "GameSession.h"
#include "Lobby.h"
#include "Match.h"
#include "Message.pb.h"

Room::Room(int roomNumber, string title, shared_ptr<GameSession> hostSession, shared_ptr<Lobby> lobby)
	: roomID_(roomNumber), title_(move(title)), lobby_(lobby)
{
	sessionSlots_.assign(4, make_pair(nullptr, false));
}

Room::~Room()
{
	GLogHelper->Reserve(LogCategory::Log_INFO, L"~Room()\n");
	sessionSlots_.clear();
	match_ = nullptr;
}

vector<pair<shared_ptr<GameSession>, bool>> Room::GetRoomInfo()
{
	vector<pair<shared_ptr<GameSession>, bool>> roomInfo;

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		roomInfo.push_back({sessionSlots_[i].first, sessionSlots_[i].second});
	}

	return roomInfo;
}

bool Room::EnterSession(shared_ptr<GameSession> session)
{
	WRITE_LOCK;
	if (numberOfPlayers_ == MAX_PLAYER_NUMBER || state_ == RoomState::INGAME)
	{
		return false;
	}

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == nullptr)
		{
			sessionSlots_[i].first = session;
			sessionSlots_[i].second = false;

			session->ProcessEnterRoom(shared_from_this());

			numberOfPlayers_++;

			return true;
		}
	}

	return false;
}

int Room::LeaveSession(const shared_ptr<GameSession>& session)
{
	WRITE_LOCK;
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == session)
		{
			sessionSlots_[i].first = nullptr;
			sessionSlots_[i].second = false;

			session->ProcessLeaveRoom();

			numberOfPlayers_--;

			return i;
		}
	}

	return -1;
}

void Room::BroadcastHere(shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;
	for (auto session : sessionSlots_)
	{
		if (session.first == nullptr)
		{
			continue;
		}
		session.first->Send(sendBuffer);
	}
}

void Room::BroadcastWithoutSelf(shared_ptr<SendBuffer> sendBuffer, const shared_ptr<GameSession>& self)
{
	WRITE_LOCK;
	for (auto session : sessionSlots_)
	{
		if (session.first == self || session.first == nullptr)
		{
			continue;
		}
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
		sessionSlots_[currentNumber].second = false;
		sessionSlots_[desireNumber].second = false;

		swap(sessionSlots_[desireNumber].first, sessionSlots_[currentNumber].first);
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

bool Room::CheckAllReady()
{
	READ_LOCK;
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == nullptr || sessionSlots_[i].second == false)
		{
			return false;
		}
	}

	return true;
}

void Room::StandByMatch(UINT count)
{
	if (count == 5 && standby_.exchange(true) == true)
	{
		return;
	}

	if (count < 5 && standby_.load() != true)
	{
		return;
	}

	{
		ProjectJ::S_ROOM_STANDBY_MATCH sendPacket;
		sendPacket.set_count(count);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		BroadcastHere(sendBuffer);
	}

	if (CheckAllReady())
	{
		if (count == 0)
		{
			StartMatch();
		}
		else
		{
			GTimerTaskManager->Reserve(1000, shared_from_this(), &Room::StandByMatch, count - 1);
		}
	}
	else
	{
		standby_.store(false);
	}
}

void Room::StartMatch()
{
	state_ = RoomState::INGAME;

	shared_ptr<GameSession> chaser = sessionSlots_[0].first;
	shared_ptr<GameSession> fugitiveFirst = sessionSlots_[1].first;
	shared_ptr<GameSession> fugitiveSecond = sessionSlots_[2].first;
	shared_ptr<GameSession> fugitiveThird = sessionSlots_[3].first;

	match_ = make_shared<Match>(shared_from_this());
	match_->Init(chaser, fugitiveFirst, fugitiveSecond, fugitiveThird);

	{
		ProjectJ::S_ROOM_START_MATCH sendPacket;
		sendPacket.set_start(true);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		BroadcastHere(sendBuffer);
	}
}

void Room::EndMatch()
{
	state_ = RoomState::WAITING;
	match_ = nullptr;
}

void Room::DestroyMatch()
{
	match_->End();
	match_ = nullptr;
}
