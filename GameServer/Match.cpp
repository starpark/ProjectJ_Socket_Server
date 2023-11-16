#include "pch.h"
#include "Match.h"

#include "DataManager.h"
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

	READ_LOCK;

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

void Match::Tick()
{
	UINT64 currentTick = GetTickCount64();
	if (currentTick >= matchEndTick_)
	{
		End();
	}

	{
		ProjectJ::S_MATCH_INFO sendPacket;
		sendPacket.set_current_tick(currentTick);

		auto info = new ProjectJ::MatchInfo();
		info->set_allocated_chaser(GetPlayerInfo(CHASER_INDEX));
		info->set_allocated_fugitive_first(GetPlayerInfo(FUGITIVE_FIRST_INDEX));
		info->set_allocated_fugitive_second(GetPlayerInfo(FUGITIVE_SECOND_INDEX));
		info->set_allocated_fugitive_third(GetPlayerInfo(FUGITIVE_THIRD_INDEX));

		sendPacket.set_allocated_info(info);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
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
	constexpr int maxWeight = 1000;

	// Init Players
	{
		auto chaserPlayer = make_shared<Player>(CHASER_INDEX, row, column, maxWeight, thisPtr);
		chaserPlayer->SetSession(chaser);
		chaser->ProcessEnterMatch(thisPtr, chaserPlayer);
		players_.push_back({chaser->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	{
		auto fugitiveFirstPlayer = make_shared<Player>(FUGITIVE_FIRST_INDEX, row, column, maxWeight, thisPtr);
		fugitiveFirstPlayer->SetSession(fugitiveFirst);
		fugitiveFirst->ProcessEnterMatch(thisPtr, fugitiveFirstPlayer);
		players_.push_back({fugitiveFirst->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	{
		auto fugitiveSecondPlayer = make_shared<Player>(FUGITIVE_SECOND_INDEX, row, column, maxWeight, thisPtr);
		fugitiveSecondPlayer->SetSession(fugitiveSecond);
		fugitiveSecond->ProcessEnterMatch(thisPtr, fugitiveSecondPlayer);
		players_.push_back({fugitiveSecond->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	{
		auto fugitiveThirdPlayer = make_shared<Player>(FUGITIVE_THIRD_INDEX, row, column, maxWeight, thisPtr);
		fugitiveThirdPlayer->SetSession(fugitiveThird);
		fugitiveThird->ProcessEnterMatch(thisPtr, fugitiveThirdPlayer);
		players_.push_back({fugitiveThird->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	// Init Scales
	for (int i = 0; i < 4; i++)
	{
		// TODO Fix Scale Data
		auto scale = make_shared<Scale>(i + SCALE_INDEX_PREFIX, 2000, 100, 15, 15);
	}

	const vector<ItemSpawnData>& spawnData = DataManager::GetItemSpawnData();
	for (int i = 0; i < spawnData.size(); i++)
	{
		shared_ptr<Item> newItem = DataManager::GenerateItem(i, spawnData[i].position_, spawnData[i].rotation_);
		items_.push_back(newItem);
	}
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
	tickHandle_ = GTimerTaskManager->AddTimer(10, true, GetMatchPtr(), &Match::Tick);
}

void Match::End()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"Room#%d Match End\n", ownerRoom_->GetID());

	GTimerTaskManager->RemoveTimer(tickHandle_);

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

bool Match::CheckPlayersState()
{
	READ_LOCK;
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
	WRITE_LOCK;
	for (auto player : players_)
	{
		if (player.first == session->GetPlayer())
		{
			player.second = state;
		}
	}
}

void Match::PlayerReadyToReceive(shared_ptr<GameSession> session)
{
	WRITE_LOCK;
	auto player = session->GetPlayer();

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i].first == player)
		{
			players_[i].second = ProjectJ::MatchPlayerState::LOADING;
		}
	}

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i].second != ProjectJ::MatchPlayerState::LOADING)
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

	ProjectJ::S_MATCH_ITEM_GENERATED itemPacket;
	for (auto newItem : items_)
	{
		auto item = itemPacket.add_items();
		item->set_id(newItem->id_);
		item->set_index(item->index());
		item->set_width(newItem->size_.x);
		item->set_height(newItem->size_.y);
		item->set_weight(newItem->weight_);
		item->set_is_owned(newItem->ownerID_.load() == Item::EMPTY_OWNER_ID);

		auto position = new ProjectJ::Vector;
		position->set_x(newItem->position_.x);
		position->set_y(newItem->position_.y);
		position->set_z(newItem->position_.z);
		item->set_allocated_world_position(position);

		auto rotation = new ProjectJ::Rotator;
		rotation->set_pitch(newItem->rotation_.pitch);
		rotation->set_roll(newItem->rotation_.roll);
		rotation->set_yaw(newItem->rotation_.yaw);
		item->set_allocated_world_rotation(rotation);

		item->set_onwer_player_index(newItem->ownerID_);
		item->set_is_rotated(newItem->bIsRotated_);
	}

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(itemPacket);
	Broadcast(sendBuffer);
}

void Match::PlayerReadyToStart(shared_ptr<GameSession> session)
{
	WRITE_LOCK;
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

	Start();
}

void Match::PlayerPickUpItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex)
{
	if (playerIndex < 0 || playerIndex >= MAX_PLAYER_NUMBER || itemIndex < 0 || itemIndex >= items_.size())
	{
		return;
	}

	shared_ptr<Player>& player = players_[playerIndex].first;
	shared_ptr<Item>& item = items_[itemIndex];

	if (player != session->GetPlayer())
	{
		return;
	}

	UINT32 expected = Item::EMPTY_OWNER_ID;
	const UINT32 desired = ((playerIndex << Item::OWNERSHIP_INDEX) & Item::OWNERSHIP_ID_MASK);
	if (item->ownerID_.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	if (player->TryAddItem(item))
	{
		item->ownerID_.store(((playerIndex & Item::OWNER_ID_MASK) | Item::OWNED_MASK), memory_order_release);

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

void Match::PlayerMoveItem(const shared_ptr<GameSession>& session, int playerIndex, int fromIndex, int toIndex, int itemIndex, int targetTopLeftIndex, bool isRotated)
{
	if (fromIndex < 0 || fromIndex >= MAX_INVENTORY_INDEX || toIndex < 0 || toIndex >= MAX_INVENTORY_INDEX ||
		itemIndex < 0 || itemIndex >= items_.size())
	{
		return;
	}

	
	shared_ptr<Player>& player = players_[playerIndex].first;
	shared_ptr<Inventory> from = fromIndex < SCALE_INDEX_PREFIX
		                             ? players_[fromIndex].first->GetInventory()
		                             : scales_[fromIndex % SCALE_INDEX_PREFIX]->GetInventory();
	shared_ptr<Inventory> to = toIndex < SCALE_INDEX_PREFIX
		                           ? players_[toIndex].first->GetInventory()
		                           : scales_[toIndex % SCALE_INDEX_PREFIX]->GetInventory();
	shared_ptr<Item>& item = items_[itemIndex];

	if (fromIndex != from->GetIndex() || toIndex != to->GetIndex() || player != session->GetPlayer())
	{
		return;
	}

	UINT32 expected = ((fromIndex & Item::OWNER_ID_MASK) | Item::OWNED_MASK);
	const UINT32 desired = (((playerIndex << Item::OWNERSHIP_INDEX) & Item::OWNERSHIP_ID_MASK) | expected);
	if (item->ownerID_.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	if (from->RelocateItem(to, item, targetTopLeftIndex, isRotated))
	{
		item->ownerID_.store(((toIndex & Item::OWNER_ID_MASK) | Item::OWNED_MASK), memory_order_release);

		ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE sendPacket;

		sendPacket.set_from_index(fromIndex);
		sendPacket.set_to_index(toIndex);
		sendPacket.set_top_left_index(item->topLeftIndex_);
		sendPacket.set_is_item_rotated(item->bIsRotated_);
		sendPacket.set_item_index(itemIndex);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);

		if (fromIndex >= SCALE_INDEX_PREFIX || fromIndex < MAX_INVENTORY_INDEX)
		{
			ProjectJ::S_MATCH_SCALE_ON_CHANGED scaleChangedPacket;
			shared_ptr<Scale> scale = static_pointer_cast<Scale>(from);

			scaleChangedPacket.set_scale_index(scale->GetIndex());
			scaleChangedPacket.set_is_operating(scale->IsOperating());
			scaleChangedPacket.set_current_weight(scale->GetCurrentWeight());

			auto scaleSendBuffer = GamePacketHandler::MakeSendBuffer(scaleChangedPacket);
			Broadcast(scaleSendBuffer);
		}

		if (toIndex >= SCALE_INDEX_PREFIX || toIndex < MAX_INVENTORY_INDEX)
		{
			ProjectJ::S_MATCH_SCALE_ON_CHANGED scaleChangedPacket;
			shared_ptr<Scale> scale = static_pointer_cast<Scale>(to);

			scaleChangedPacket.set_scale_index(scale->GetIndex());
			scaleChangedPacket.set_is_operating(scale->IsOperating());
			scaleChangedPacket.set_current_weight(scale->GetCurrentWeight());

			auto scaleSendBuffer = GamePacketHandler::MakeSendBuffer(scaleChangedPacket);
			Broadcast(scaleSendBuffer);
		}
	}
	else
	{
		item->ownerID_.store(((fromIndex & Item::OWNER_ID_MASK) | Item::OWNED_MASK), memory_order_release);
	}
}

void Match::PlayerDropItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex, ProjectJ::Vector position, ProjectJ::Rotator rotation)
{
	if (playerIndex < 0 || playerIndex >= MAX_PLAYER_NUMBER || itemIndex < 0 || itemIndex >= items_.size())
	{
		return;
	}

	shared_ptr<Player>& player = players_[playerIndex].first;
	shared_ptr<Item>& item = items_[itemIndex];

	if (player != session->GetPlayer())
	{
		return;
	}

	const UINT32 origin = item->ownerID_.load();
	UINT32 expected = ((playerIndex & Item::OWNER_ID_MASK) | Item::OWNED_MASK);
	const UINT32 desired = (((playerIndex << Item::OWNERSHIP_INDEX) & Item::OWNERSHIP_ID_MASK) | expected);
	if(item->ownerID_.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	if(player->DropItem(item, position, rotation))
	{
		item->ownerID_.store(Item::EMPTY_OWNER_ID, memory_order_release);

		ProjectJ::S_MATCH_ITEM_SOMEONE_DROP sendPacket;

		sendPacket.set_item_index(itemIndex);
		sendPacket.set_player_index(playerIndex);
		sendPacket.set_allocated_drop_item_position(new ProjectJ::Vector(position));
		sendPacket.set_allocated_drop_item_rotation(new ProjectJ::Rotator(rotation));

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
	else
	{
		item->ownerID_.store((origin & (Item::OWNED_MASK | Item::OWNER_ID_MASK)), memory_order_release);
	}
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
