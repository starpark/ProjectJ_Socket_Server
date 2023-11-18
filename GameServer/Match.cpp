#include "pch.h"
#include "Match.h"
#include "DataManager.h"
#include "GameSession.h"
#include "Room.h"
#include "Item.h"
#include "GamePacketHandler.h"

#define _USE_MATH_DEFINES
#include <math.h>

Match::Match(shared_ptr<Room> owner)
	: ownerRoom_(owner)
{
}

Match::~Match()
{
	// Call After End
	GLogHelper->Print(LogCategory::LOG_INFO, L"~Match()\n");

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

	position->set_x(vector.x_);
	position->set_y(vector.y_);
	position->set_z(vector.z_);

	rotation->set_pitch(rotator.pitch_);
	rotation->set_roll(rotator.roll_);
	rotation->set_yaw(rotator.yaw_);

	player->set_account_id(playerPtr->GetID());
	player->set_nickname(playerPtr->GetNickname());

	playerInfo->set_state(players_[playerIndex].second);
	playerInfo->set_player_index(playerIndex);
	playerInfo->set_allocated_player(player);
	playerInfo->set_allocated_position(position);
	playerInfo->set_allocated_rotation(rotation);

	return playerInfo;
}

void Match::Tick()
{
	if (isMatchEnded_.load() != false)
	{
		return;
	}

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


void Match::Init(shared_ptr<GameSession> chaser, shared_ptr<GameSession> fugitiveFirst,
                 shared_ptr<GameSession> fugitiveSecond,
                 shared_ptr<GameSession> fugitiveThird)
{
	WRITE_LOCK;
	GLogHelper->Print(LogCategory::LOG_INFO, L"Room#%d Match Initialized\n", ownerRoom_->GetID());
	shared_ptr<Match> thisPtr = static_pointer_cast<Match>(shared_from_this());

	constexpr int row = 15;
	constexpr int column = 6;
	constexpr int maxWeight = 1000;

	// Init Players
	{
		auto chaserPlayer = make_shared<Player>(CHASER_INDEX, row, column, maxWeight, chaser->GetID(), chaser->GetNickname());
		chaserPlayer->SetSession(chaser);
		chaser->ProcessEnterMatch(thisPtr, chaserPlayer);
		players_.push_back({chaser->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	{
		auto fugitiveFirstPlayer = make_shared<Player>(FUGITIVE_FIRST_INDEX, row, column, maxWeight, fugitiveFirst->GetID(),
		                                               fugitiveFirst->GetNickname());
		fugitiveFirstPlayer->SetSession(fugitiveFirst);
		fugitiveFirst->ProcessEnterMatch(thisPtr, fugitiveFirstPlayer);
		players_.push_back({fugitiveFirst->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	{
		auto fugitiveSecondPlayer = make_shared<Player>(FUGITIVE_SECOND_INDEX, row, column, maxWeight, fugitiveSecond->GetID(),
		                                                fugitiveSecond->GetNickname());
		fugitiveSecondPlayer->SetSession(fugitiveSecond);
		fugitiveSecond->ProcessEnterMatch(thisPtr, fugitiveSecondPlayer);
		players_.push_back({fugitiveSecond->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	{
		auto fugitiveThirdPlayer = make_shared<Player>(FUGITIVE_THIRD_INDEX, row, column, maxWeight, fugitiveThird->GetID(),
		                                               fugitiveThird->GetNickname());
		fugitiveThirdPlayer->SetSession(fugitiveThird);
		fugitiveThird->ProcessEnterMatch(thisPtr, fugitiveThirdPlayer);
		players_.push_back({fugitiveThird->GetPlayer(), ProjectJ::MatchPlayerState::NONE});
	}

	// Init Scales
	for (int i = 0; i < 4; i++)
	{
		// TODO Fix Scale Data
		auto scale = make_shared<Scale>(i + SCALE_INDEX_PREFIX, 2000, 100, 15, 15);
		scales_.push_back(move(scale));
	}

	const vector<ItemSpawnData>& spawnData = DataManager::GetItemSpawnData();
	for (int i = 0; i < spawnData.size(); i++)
	{
		shared_ptr<Item> newItem = DataManager::GenerateItem(i, spawnData[i].position_, spawnData[i].rotation_);
		items_.push_back(move(newItem));
	}

	timeOutHandle_ = GTimerTaskManager->AddTimer(START_TIME_OUT, false, this, &Match::StartTimeOut);
}


void Match::Start()
{
	GLogHelper->Print(LogCategory::LOG_INFO, L"Room#%d Match Start\n", ownerRoom_->GetID());

	ASSERT_CRASH(isMatchStarted_.exchange(true) == false);

	GTimerTaskManager->RemoveTimer(timeOutHandle_);

	const UINT64 currentTick = GetTickCount64();
	matchEndTick_ = currentTick + MATCH_END_TICK;

	ProjectJ::S_MATCH_START sendPacket;
	sendPacket.set_current_tick(currentTick);
	sendPacket.set_end_tick(matchEndTick_);

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	Broadcast(sendBuffer);

	// TODO TICK
	tickHandle_ = GTimerTaskManager->AddTimer(15, true, this, &Match::Tick);
}

void Match::End()
{
	if (isMatchEnded_.exchange(true) == true)
	{
		return;
	}

	GLogHelper->Print(LogCategory::LOG_INFO, L"Room#%d Match Ended\n", ownerRoom_->GetID());

	GTimerTaskManager->RemoveTimer(tickHandle_);
	GTimerTaskManager->RemoveTimer(timeOutHandle_);

	// TODO Score Calculation
	ProjectJ::S_MATCH_END sendPacket;

	sendPacket.set_chaser_score(0);
	sendPacket.set_fugitivie_first_score(1);
	sendPacket.set_fugitivie_first_score(2);
	sendPacket.set_fugitivie_first_score(3);

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	Broadcast(sendBuffer);

	PlayerBackToRoom();
	ownerRoom_->DoTaskAsync(&Room::EndMatch);
}

void Match::StartTimeOut()
{
	if (isMatchStarted_.load() == false)
	{
		GLogHelper->Print(LogCategory::LOG_INFO, L"Room#%d Match Start Time Out\n", ownerRoom_->GetID());

		End();
	}
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

	GLogHelper->Print(LogCategory::LOG_INFO, L"Room#%d Match Ready To Receive\n", ownerRoom_->GetID());

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
		item->set_id(newItem->id);
		item->set_index(item->index());
		item->set_width(newItem->size.x);
		item->set_height(newItem->size.y);
		item->set_weight(newItem->weight);
		item->set_is_owned(false);

		auto position = new ProjectJ::Vector;
		position->set_x(newItem->position.x_);
		position->set_y(newItem->position.y_);
		position->set_z(newItem->position.z_);
		item->set_allocated_world_position(position);

		auto rotation = new ProjectJ::Rotator;
		rotation->set_pitch(newItem->rotation.pitch_);
		rotation->set_roll(newItem->rotation.roll_);
		rotation->set_yaw(newItem->rotation.yaw_);
		item->set_allocated_world_rotation(rotation);

		item->set_onwer_player_index(INVALID_ITEM_OWNER_ID);
		item->set_is_rotated(newItem->bIsRotated);
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
	const UINT32 desired = (Item::HANDLE_MASK | ((playerIndex << Item::HANDLE_PLAYER_INDEX) & Item::HANDLE_PLAYER_MASK));
	if (item->ownerFlag.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	InventoryErrorCode errorCode = player->TryAddItem(item);

	if (errorCode == InventoryErrorCode::SUCCESS)
	{
		item->ownerFlag.store((Item::OWNED_MASK | (playerIndex & Item::OWNER_PLAYER_MASK)), memory_order_release);

		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"Room#%d Match Player#%d Pick Up Item#%d Item ID: %d\n",
		                  ownerRoom_->GetID(), playerIndex, itemIndex, item->id);

		ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP sendPacket;

		sendPacket.set_is_item_rotated(item->bIsRotated);
		sendPacket.set_item_index(itemIndex);
		sendPacket.set_player_index(playerIndex);
		sendPacket.set_top_left_index(item->topLeftIndex);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
	else
	{
		item->ownerFlag.store(Item::EMPTY_OWNER_ID, memory_order_release);

		GLogHelper->Print(LogCategory::LOG_WARN,
		                  L"Room#%d Match Player#%d Fail To Pick Up Item#%d: %s\n",
		                  ownerRoom_->GetID(), playerIndex, itemIndex, Inventory::GetErrorWhat(errorCode));
	}
}

void Match::PlayerMoveItem(const shared_ptr<GameSession>& session, int playerIndex, int fromIndex, int toIndex, int itemIndex, int targetTopLeftIndex,
                           bool isRotated)
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

	UINT32 expected = (Item::OWNED_MASK | (fromIndex & Item::OWNER_PLAYER_MASK));
	const UINT32 itemHandleFlag = (Item::HANDLE_MASK | ((playerIndex << Item::HANDLE_PLAYER_INDEX) & Item::HANDLE_PLAYER_MASK));
	const UINT32 desired = (itemHandleFlag | expected);
	if (item->ownerFlag.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	InventoryErrorCode errorCode = from->RelocateItem(to, item, targetTopLeftIndex, isRotated);
	if (errorCode == InventoryErrorCode::SUCCESS)
	{
		item->ownerFlag.store((Item::OWNED_MASK | (toIndex & Item::OWNER_PLAYER_MASK)), memory_order_release);

		ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE sendPacket;

		sendPacket.set_from_index(fromIndex);
		sendPacket.set_to_index(toIndex);
		sendPacket.set_top_left_index(item->topLeftIndex);
		sendPacket.set_is_item_rotated(item->bIsRotated);
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
		item->ownerFlag.store((Item::OWNED_MASK | (fromIndex & Item::OWNER_PLAYER_MASK)), memory_order_release);

		GLogHelper->Print(LogCategory::LOG_WARN,
		                  L"Room#%d Match Player#%d Fail To Move Item#%d: %s\n",
		                  ownerRoom_->GetID(), playerIndex, itemIndex, Inventory::GetErrorWhat(errorCode));
	}
}

void Match::PlayerDropItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex, Vector position,
                           Rotator rotation)
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

	const UINT32 origin = item->ownerFlag.load();
	UINT32 expected = (Item::OWNED_MASK | (playerIndex & Item::OWNER_PLAYER_MASK));
	const UINT32 itemHandleFlag = (Item::HANDLE_MASK | ((playerIndex << Item::HANDLE_PLAYER_INDEX) & Item::HANDLE_PLAYER_MASK));
	const UINT32 desired = (itemHandleFlag | expected);
	if (item->ownerFlag.compare_exchange_strong(expected, desired, memory_order_acquire) == false)
	{
		return;
	}

	InventoryErrorCode errorCode = player->DropItem(item, position, rotation);
	if (errorCode == InventoryErrorCode::SUCCESS)
	{
		item->ownerFlag.store(Item::EMPTY_OWNER_ID, memory_order_release);

		ProjectJ::S_MATCH_ITEM_SOMEONE_DROP sendPacket;

		sendPacket.set_item_index(itemIndex);
		sendPacket.set_player_index(playerIndex);

		auto dropPosition = new ProjectJ::Vector();
		dropPosition->set_x(item->position.x_);
		dropPosition->set_y(item->position.y_);
		dropPosition->set_z(item->position.z_);
		sendPacket.set_allocated_drop_item_position(dropPosition);

		auto dropRotation = new ProjectJ::Rotator();
		dropRotation->set_pitch(item->rotation.pitch_);
		dropRotation->set_roll(item->rotation.roll_);
		dropRotation->set_yaw(item->rotation.yaw_);
		sendPacket.set_allocated_drop_item_rotation(dropRotation);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
	else
	{
		item->ownerFlag.store((origin & (Item::OWNED_MASK | Item::OWNER_PLAYER_MASK)), memory_order_release);

		GLogHelper->Print(LogCategory::LOG_WARN,
		                  L"Room#%d Match Player#%d Fail To Drop Item#%d: %s\n",
		                  ownerRoom_->GetID(), playerIndex, itemIndex, Inventory::GetErrorWhat(errorCode));
	}
}

void Match::ChaserAttack(const shared_ptr<GameSession>& session, Vector position, Rotator rotation)
{
	if (players_[CHASER_INDEX].first != session->GetPlayer())
	{
		session->Disconnect();
	}

	// TODO BRAODCAST
}

void Match::HitValidation(const shared_ptr<GameSession>& session, Vector position, Rotator rotation, int targetPlayerIndex)
{
	if (targetPlayerIndex < 1 || targetPlayerIndex >= MAX_PLAYER_NUMBER || players_[CHASER_INDEX].first != session->GetPlayer())
	{
		session->Disconnect();
		return;
	}

	Vector chaserPosition(position), targetPosition{0,};
	Rotator chaserRotation(rotation);
	{
		READ_LOCK;
		targetPosition = players_[targetPlayerIndex].first->GetVector();
	}

	float distance = Vector::Distance(chaserPosition, targetPosition);

	Vector chaserVector2D = chaserRotation.Vector2D().Normalize2D();
	Vector toTargetVector2D = (targetPosition - chaserPosition).Normalize2D();

	float angle = Vector::Angle(chaserVector2D, toTargetVector2D);

	// TODO Validate Hit
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
