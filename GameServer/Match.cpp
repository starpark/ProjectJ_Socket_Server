#include "pch.h"
#include "Match.h"
#include "GameSession.h"
#include "Room.h"

Match::Match(shared_ptr<GameSession> chaser, shared_ptr<GameSession> fugitiveOne, shared_ptr<GameSession> fugitiveTwo,
             shared_ptr<GameSession> fugitiveThree, shared_ptr<Room> owner)
	: ownerRoom_(owner)
{
	shared_ptr<Match> thisPtr = static_pointer_cast<Match>(shared_from_this());

	auto chaserPlayer = make_shared<Player>(thisPtr);
	chaser->ProcessEnterMatch(thisPtr, chaserPlayer);
	players_.push_back({chaser->GetPlayer(), true});

	auto fugitiveOnePlayer = make_shared<Player>(thisPtr);
	fugitiveOne->ProcessEnterMatch(thisPtr, fugitiveOnePlayer);
	players_.push_back({fugitiveOne->GetPlayer(), true});


	auto fugitiveTwoPlayer = make_shared<Player>(thisPtr);
	fugitiveTwo->ProcessEnterMatch(thisPtr, fugitiveTwoPlayer);
	players_.push_back({fugitiveTwo->GetPlayer(), true});


	auto fugitiveThreePlayer = make_shared<Player>(thisPtr);
	fugitiveThree->ProcessEnterMatch(thisPtr, fugitiveThreePlayer);
	players_.push_back({fugitiveThree->GetPlayer(), true});
}

Match::~Match()
{
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (auto session = players_[i].first->GetOwnerSession())
		{
			session->ProcessLeaveMatch();
		}
	}
	players_.clear();
	itemManager_ = nullptr;
}

void Match::Tick(double deltaTime)
{
}

void Match::MatchInit()
{
}

void Match::MatchStart()
{
}

void Match::MatchEnd()
{
}

void Match::PlayerDisconnected(const shared_ptr<Player>& disconnectedPlayer)
{
	WRITE_LOCK;
	for (auto player : players_)
	{
		if (player.first == disconnectedPlayer)
		{
			player.second = false;
		}
	}
}

void Match::BroadcastMatchInfo()
{
	WRITE_LOCK;
	for (auto player : players_)
	{
		if (auto session = player.first->GetOwnerSession())
		{
			// session->Send()
		}
	}
}
