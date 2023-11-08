#include "pch.h"
#include "Match.h"
#include "GameSession.h"
#include "Room.h"
#include "Item.h"

Match::Match(shared_ptr<Room> owner)
	: ownerRoom_(owner)
{
}

Match::~Match()
{
	GLogHelper->Reserve(LogCategory::Log_INFO, "~Match()\n");
	ownerRoom_ = nullptr;
	players_.clear();
	items_.clear();
}

void Match::Tick(double deltaTime)
{
	// GLogHelper->SetTimer(LogCategory::Log_INFO, "Room#%d Match Tick \n", ownerRoom_->GetID());
	if (CheckEndCondition())
	{
		End();
	}
}

void Match::Init(shared_ptr<GameSession> chaser, shared_ptr<GameSession> fugitiveFirst,
                 shared_ptr<GameSession> fugitiveSecond,
                 shared_ptr<GameSession> fugitiveThird)
{
	GLogHelper->Reserve(LogCategory::Log_INFO, "Room#%d Init Match \n", ownerRoom_->GetID());
	shared_ptr<Match> thisPtr = static_pointer_cast<Match>(shared_from_this());

	{
		WRITE_LOCK;
		auto chaserPlayer = make_shared<Player>(thisPtr);
		chaserPlayer->SetSession(chaser);
		chaser->ProcessEnterMatch(thisPtr, chaserPlayer);
		players_.push_back({chaser->GetPlayer(), PlayerState::LOADING});

		auto fugitiveFirstPlayer = make_shared<Player>(thisPtr);
		fugitiveFirstPlayer->SetSession(fugitiveFirst);
		fugitiveFirst->ProcessEnterMatch(thisPtr, fugitiveFirstPlayer);
		players_.push_back({fugitiveFirst->GetPlayer(), PlayerState::LOADING});

		auto fugitiveSecondPlayer = make_shared<Player>(thisPtr);
		fugitiveSecondPlayer->SetSession(fugitiveSecond);
		fugitiveSecond->ProcessEnterMatch(thisPtr, fugitiveSecondPlayer);
		players_.push_back({fugitiveSecond->GetPlayer(), PlayerState::LOADING});


		auto fugitiveThirdPlayer = make_shared<Player>(thisPtr);
		fugitiveThirdPlayer->SetSession(fugitiveThird);
		fugitiveThird->ProcessEnterMatch(thisPtr, fugitiveThirdPlayer);
		players_.push_back({fugitiveThird->GetPlayer(), PlayerState::LOADING});
	}

	// TEST START
	// TODO
	Start();
}


void Match::Start()
{
	/*{
		READ_LOCK;

		for (auto& player : players_)
		{
			if (player.second == PlayerState::LOADING)
			{
				return;
			}
		}
	}*/

	if (matchStated_.exchange(true) == true)
	{
		return;
	}

	GLogHelper->Reserve(LogCategory::Log_INFO, "Room#%d Match Start \n", ownerRoom_->GetID());
	const UINT64 currentTick = GetTickCount64();
	matchEndTick_ = currentTick + MATCH_END_TICK;

	// TODO TICK
}

void Match::End()
{
	GLogHelper->Reserve(LogCategory::Log_INFO, L"Room#%d Match End\n", ownerRoom_->GetID());
	// TODO Score Calculation
	if (matchStated_.load() == true)
	{
		matchStated_.store(false);
	}

	PlayerBackToRoom();
	players_.clear();
	items_.clear();

	ownerRoom_->SetState(RoomState::WAITING);
	ownerRoom_->EndMatch();
	ownerRoom_ = nullptr;
}

bool Match::CheckEndCondition()
{
	// Match End Tick
	UINT64 currentTick = GetTickCount64();
	if (currentTick >= matchEndTick_)
	{
		return true;
	}

	return false;
}

bool Match::CheckPlayers()
{
	{
		READ_LOCK;
		// Chaser Disconnected
		if (players_[0].second == PlayerState::DISCONNECTED)
		{
			return true;
		}
	}

	{
		READ_LOCK;
		// All Fugitives Disconnected
		bool flag = false;
		for (int i = 1; i < MAX_PLAYER_NUMBER; i++)
		{
			flag |= (players_[i].second == PlayerState::DISCONNECTED ? false : true);
		}

		return flag == false;
	}
}

void Match::PlayerStateChanged(shared_ptr<GameSession> session, PlayerState state)
{
	WRITE_LOCK;
	for (auto player : players_)
	{
		if (player.first == session->GetPlayer())
		{
			player.second = state;
		}
	}
}


void Match::PlayerDisconnected(const shared_ptr<GameSession>& session)
{
	WRITE_LOCK;
	for (auto& player : players_)
	{
		if (player.first == session->GetPlayer())
		{
			player.second = PlayerState::DISCONNECTED;
			session->ProcessLeaveMatch();

			if (CheckPlayers())
			{
				End();
			}

			return;
		}
	}
}

void Match::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;
	for (auto& player : players_)
	{
		if (player.second == PlayerState::DISCONNECTED)
		{
			continue;
		}

		if (auto session = player.first->GetOwnerSession())
		{
			session->Send(sendBuffer);
		}
	}
}

void Match::PlayerBackToRoom()
{
	WRITE_LOCK;
	for (auto& player : players_)
	{
		if (auto session = player.first->GetOwnerSession())
		{
			if (player.second != PlayerState::DISCONNECTED)
			{
				session->ProcessLeaveMatch();
			}

			player.second = PlayerState::NONE;
		}
	}
}

void Match::GenerateItems()
{
	int itemIndex = 1;
}
