#include "pch.h"
#include "Match.h"
#include "GameSession.h"
#include "Room.h"
#include "Item.h"
#include "GamePacketHandler.h"

Match::Match(shared_ptr<Room> owner)
	: ownerRoom_(owner)
{
}

Match::~Match()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"~Match()\n");
	ownerRoom_ = nullptr;
	players_.clear();
	items_.clear();
}

ProjectJ::PlayerInfo* Match::GetPlayerInfo(int playerIndex)
{
	if (playerIndex < 0 || playerIndex >= MAX_PLAYER_NUMBER)
	{
		return nullptr;
	}

	auto playerPtr = players_[playerIndex].first;

	auto playerInfo = new ProjectJ::PlayerInfo();
	auto position = new ProjectJ::Vector();
	auto rotation = new ProjectJ::Rotator();
	auto player = new ProjectJ::Player();

	Vector vector = playerPtr->GetVector();
	Rotator rotator = playerPtr->GetRotator();

	position->set_x(vector.x);
	position->set_y(vector.y);
	position->set_z(vector.z);

	rotation->set_pitch(rotator.pitch);
	rotation->set_roll(rotator.roll);
	rotation->set_yaw(rotator.yaw);

	player->set_account_id(playerPtr->GetOwnerSession()->GetID());
	player->set_nickname(playerPtr->GetOwnerSession()->GetNickname());

	playerInfo->set_state(players_[playerIndex].second);
	playerInfo->set_player_index(playerIndex);
	playerInfo->set_allocated_player(player);
	playerInfo->set_allocated_position(position);
	playerInfo->set_allocated_rotation(rotation);
}

void Match::Tick(double deltaTime)
{
	UINT64 currentTick = GetTickCount64();
	if (currentTick >= matchEndTick_)
	{
		End();
	}
}

void Match::Init(shared_ptr<GameSession> chaser, shared_ptr<GameSession> fugitiveFirst,
                 shared_ptr<GameSession> fugitiveSecond,
                 shared_ptr<GameSession> fugitiveThird)
{
	GLogHelper->Print(LogCategory::Log_INFO, L"Room#%d Init Match \n", ownerRoom_->GetID());
	shared_ptr<Match> thisPtr = static_pointer_cast<Match>(shared_from_this());

	constexpr int row = 15;
	constexpr int column = 6;
	constexpr int maxWeight = 5000;

	// Init Players
	{
		auto chaserPlayer = make_shared<Player>(CHASER_INDEX, row, column, maxWeight, thisPtr);
		chaserPlayer->SetSession(chaser);
		chaser->ProcessEnterMatch(thisPtr, chaserPlayer);
		players_.push_back({chaser->GetPlayer(), ProjectJ::MatchPlayerState::LOADING});
	}

	{
		auto fugitiveFirstPlayer = make_shared<Player>(FUGITIVE_FIRST_INDEX, row, column, maxWeight, thisPtr);
		fugitiveFirstPlayer->SetSession(fugitiveFirst);
		fugitiveFirst->ProcessEnterMatch(thisPtr, fugitiveFirstPlayer);
		players_.push_back({fugitiveFirst->GetPlayer(), ProjectJ::MatchPlayerState::LOADING});
	}

	{
		auto fugitiveSecondPlayer = make_shared<Player>(FUGITIVE_SECOND_INDEX, row, column, maxWeight, thisPtr);
		fugitiveSecondPlayer->SetSession(fugitiveSecond);
		fugitiveSecond->ProcessEnterMatch(thisPtr, fugitiveSecondPlayer);
		players_.push_back({fugitiveSecond->GetPlayer(), ProjectJ::MatchPlayerState::LOADING});
	}

	{
		auto fugitiveThirdPlayer = make_shared<Player>(FUGITIVE_THIRD_INDEX, row, column, maxWeight, thisPtr);
		fugitiveThirdPlayer->SetSession(fugitiveThird);
		fugitiveThird->ProcessEnterMatch(thisPtr, fugitiveThirdPlayer);
		players_.push_back({fugitiveThird->GetPlayer(), ProjectJ::MatchPlayerState::LOADING});
	}

	// Init Scales
	for (int i = 0; i < 4; i++)
	{
		// TODO Fix Scale Data
		auto scale = make_shared<Scale>(i, 100000, 10000, 20, 20);
	}

	// TODO Generate Items
	for (int i = 0; i < 100; i++)
	{
	}
	// TODO Send Items Data
}


void Match::Start()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"Room#%d Match Start \n", ownerRoom_->GetID());
	const UINT64 currentTick = GetTickCount64();
	matchEndTick_ = currentTick + MATCH_END_TICK;

	ProjectJ::S_MATCH_START sendPacket;
	sendPacket.set_current_tick(currentTick);
	sendPacket.set_end_tick(matchEndTick_);

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	Broadcast(sendBuffer);

	// TODO TICK
}

void Match::End()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"Room#%d Match End\n", ownerRoom_->GetID());
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

bool Match::CheckPlayersState()
{
	{
		// Check Chaser Disconnected
		if (players_[CHASER_INDEX].second == ProjectJ::MatchPlayerState::DISCONNECTED)
		{
			return true;
		}
	}

	{
		// Check All Fugitives Disconnected
		bool flag = false;
		for (int i = FUGITIVE_FIRST_INDEX; i <= FUGITIVE_THIRD_INDEX; i++)
		{
			flag |= (players_[i].second == ProjectJ::MatchPlayerState::DISCONNECTED ? false : true);
		}

		return flag == false;
	}
}

void Match::PlayerStateChanged(const shared_ptr<GameSession>& session, ProjectJ::MatchPlayerState state)
{
	for (auto player : players_)
	{
		if (player.first == session->GetPlayer())
		{
			player.second = state;
		}
	}
}

void Match::PlayerLoadingComplete(shared_ptr<GameSession> session)
{
	auto player = session->GetPlayer();

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i].first == player)
		{
			players_[i].second = ProjectJ::MatchPlayerState::ALIVE;
		}
	}

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i].second != ProjectJ::MatchPlayerState::ALIVE)
		{
			return;
		}
	}

	ProjectJ::S_MATCH_ALL_READY_TO_RECIEVE sendPacket;
	auto initInfo = new ProjectJ::MatchInitInfo();

	initInfo->set_allocated_chaser(GetPlayerInfo(CHASER_INDEX));
	initInfo->set_allocated_fugitive_first(GetPlayerInfo(FUGITIVE_FIRST_INDEX));
	initInfo->set_allocated_fugitive_second(GetPlayerInfo(FUGITIVE_SECOND_INDEX));
	initInfo->set_allocated_fugitive_third(GetPlayerInfo(FUGITIVE_THIRD_INDEX));

	// Scale first
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_FIRST_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_FIRST_INDEX]->GetTolerance());
		initInfo->set_allocated_scale_first(scaleInfo);
	}

	// Scale second 
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_SECOND_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_SECOND_INDEX]->GetTolerance());
		initInfo->set_allocated_scale_second(scaleInfo);
	}

	// Scale third
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_THIRD_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_THIRD_INDEX]->GetTolerance());
		initInfo->set_allocated_scale_third(scaleInfo);
	}

	// Scale fourth
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_FOURTH_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_FOURTH_INDEX]->GetTolerance());
		initInfo->set_allocated_scale_fourth(scaleInfo);
	}

	sendPacket.set_allocated_info(initInfo);

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		sendPacket.set_your_player_index(i);
		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);

		players_[i].first->GetOwnerSession()->Send(sendBuffer);
	}
}

void Match::PlayerPickupItem(int playerIndex, int itemIndex)
{
	if (playerIndex < 0 || playerIndex >= MAX_PLAYER_NUMBER || itemIndex < 0 || itemIndex >= items_.size())
	{
		return;
	}

	shared_ptr<Player>& player = players_[playerIndex].first;
	shared_ptr<Item>& item = items_[itemIndex];

	UINT32 expected = Item::EMPTY_OWNER_ID;
	const UINT32 desired = ((playerIndex << 16) & Item::OWNERSHIP_ID_MASK);
	if (item->ownerID_.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	if (player->TryAddItem(item))
	{
		item->ownerID_.store((playerIndex & Item::OWNER_ID_MASK), memory_order_release);

		ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP sendPacket;

		sendPacket.set_is_item_rotated(item->bIsRotated_);
		sendPacket.set_item_index(itemIndex);
		sendPacket.set_player_index(playerIndex);
		sendPacket.set_top_left_index(item->topLeftIndex_);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
	else
	{
		item->ownerID_.store(Item::EMPTY_OWNER_ID, memory_order_release);
	}
}

void Match::PlayerMoveItem(int playerIndex, int fromIndex, int toIndex, int itemIndex, int targetTopLeftIndex, bool isRotated)
{
	if (fromIndex < 0 || fromIndex >= MAX_INVENTORY_INDEX || toIndex < 0 || toIndex >= MAX_INVENTORY_INDEX ||
		itemIndex < 0 || itemIndex >= items_.size())
	{
		return;
	}

	shared_ptr<Inventory> from = fromIndex <= FUGITIVE_THIRD_INDEX
		                             ? players_[fromIndex].first->GetInventory()
		                             : scales_[fromIndex % 4]->GetInventory();
	shared_ptr<Inventory> to = toIndex <= FUGITIVE_THIRD_INDEX
		                           ? players_[toIndex].first->GetInventory()
		                           : scales_[toIndex % 4]->GetInventory();
	shared_ptr<Item>& item = items_[itemIndex];

	UINT32 expected = (item->ownerID_.load() & Item::OWNER_ID_MASK);
	const UINT32 desired = ((playerIndex << 16) | expected);
	if (item->ownerID_.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	if (from->RelocateItem(to, item, targetTopLeftIndex, isRotated))
	{
		item->ownerID_.store((toIndex & Item::OWNER_ID_MASK), memory_order_release);

		ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE sendPacket;

		sendPacket.set_from_index(fromIndex);
		sendPacket.set_to_index(toIndex);
		sendPacket.set_top_left_index(item->topLeftIndex_);
		sendPacket.set_is_item_rotated(item->bIsRotated_);
		sendPacket.set_item_index(itemIndex);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
	else
	{
		item->ownerID_.store((fromIndex & Item::OWNER_ID_MASK), memory_order_release);
	}
}

void Match::PlayerDropItem(int playerIndex, int itemIndex, ProjectJ::Vector position, ProjectJ::Rotator rotation)
{
}


void Match::PlayerDisconnected(const shared_ptr<GameSession>& session)
{
	WRITE_LOCK;
	for (auto& player : players_)
	{
		if (player.first == session->GetPlayer())
		{
			player.second = ProjectJ::MatchPlayerState::DISCONNECTED;
			session->ProcessLeaveMatch();

			if (CheckPlayersState())
			{
				End();
			}

			return;
		}
	}
}

void Match::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	READ_LOCK;
	for (auto& player : players_)
	{
		if (player.second == ProjectJ::MatchPlayerState::DISCONNECTED)
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
			if (player.second != ProjectJ::MatchPlayerState::DISCONNECTED)
			{
				session->ProcessLeaveMatch();
			}

			player.second = ProjectJ::MatchPlayerState::NONE;
		}
	}
}

void Match::GenerateItems()
{
}
