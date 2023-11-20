#include "pch.h"
#include "Match.h"
#include "DataManager.h"
#include "GameSession.h"
#include "Room.h"
#include "Item.h"
#include "GamePacketHandler.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"


Match::Match(shared_ptr<Room> owner)
	: ownerRoom_(owner)
{
	players_.resize(MAX_PLAYER_NUMBER);
	scales_.resize(MAX_SCALE_NUMBER);
}

Match::~Match()
{
	// Call After End
	GLogHelper->Print(LogCategory::LOG_INFO, L"~Match()\n");

	ownerRoom_ = nullptr;
	players_.clear();
	items_.clear();
	scales_.clear();
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
		info->set_allocated_chaser(players_[CHASER_INDEX]->GetPlayerInfo());
		info->set_allocated_fugitive_first(players_[FUGITIVE_FIRST_INDEX]->GetPlayerInfo());
		info->set_allocated_fugitive_second(players_[FUGITIVE_SECOND_INDEX]->GetPlayerInfo());
		info->set_allocated_fugitive_third(players_[FUGITIVE_THIRD_INDEX]->GetPlayerInfo());

		sendPacket.set_allocated_info(info);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
}

void Match::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	for (auto& player : players_)
	{
		if (player->GetState() == ProjectJ::DISCONNECTED)
		{
			continue;
		}

		if (auto session = player->GetOwnerSession())
		{
			session->Send(sendBuffer);
		}
	}
}


void Match::Init(shared_ptr<GameSession> chaser, shared_ptr<GameSession> fugitiveFirst,
                 shared_ptr<GameSession> fugitiveSecond,
                 shared_ptr<GameSession> fugitiveThird)
{
	shared_ptr<Match> thisPtr = static_pointer_cast<Match>(shared_from_this());

	constexpr int row = 15;
	constexpr int column = 6;
	constexpr int maxWeight = 1000;

	// Init Players
	{
		auto chaserPlayer = make_shared<Player>(CHASER_INDEX, row, column, maxWeight, chaser->GetID(), chaser->GetNickname());
		chaserPlayer->SetSession(chaser);
		chaser->ProcessEnterMatch(thisPtr, chaserPlayer);
		players_[CHASER_INDEX] = chaserPlayer;
	}

	{
		auto fugitiveFirstPlayer = make_shared<Player>(FUGITIVE_FIRST_INDEX, row, column, maxWeight, fugitiveFirst->GetID(),
		                                               fugitiveFirst->GetNickname());
		fugitiveFirstPlayer->SetSession(fugitiveFirst);
		fugitiveFirst->ProcessEnterMatch(thisPtr, fugitiveFirstPlayer);
		players_[FUGITIVE_FIRST_INDEX] = fugitiveFirstPlayer;
	}

	{
		auto fugitiveSecondPlayer = make_shared<Player>(FUGITIVE_SECOND_INDEX, row, column, maxWeight, fugitiveSecond->GetID(),
		                                                fugitiveSecond->GetNickname());
		fugitiveSecondPlayer->SetSession(fugitiveSecond);
		fugitiveSecond->ProcessEnterMatch(thisPtr, fugitiveSecondPlayer);
		players_[FUGITIVE_SECOND_INDEX] = fugitiveSecondPlayer;
	}

	{
		auto fugitiveThirdPlayer = make_shared<Player>(FUGITIVE_THIRD_INDEX, row, column, maxWeight, fugitiveThird->GetID(),
		                                               fugitiveThird->GetNickname());
		fugitiveThirdPlayer->SetSession(fugitiveThird);
		fugitiveThird->ProcessEnterMatch(thisPtr, fugitiveThirdPlayer);
		players_[FUGITIVE_THIRD_INDEX] = fugitiveThirdPlayer;
	}

	// Init Scales
	for (int i = 0; i < 4; i++)
	{
		// TODO Fix Scale Data
		auto scale = make_shared<Scale>(i + MAX_SCALE_NUMBER, 2000, 100, SCALE_HEIGHT, SCALE_WIDTH);
		scales_[i] = scale;
	}

	const vector<ItemSpawnData>& spawnData = DataManager::GetItemSpawnData();
	for (int i = 0; i < spawnData.size(); i++)
	{
		shared_ptr<Item> newItem = DataManager::GenerateItem(i, spawnData[i].position_, spawnData[i].rotation_);
		items_.push_back(move(newItem));
	}


	timeOutHandle_ = GTimerTaskManager->AddTimer(START_TIME_OUT, false, this, &Match::StartTimeOut);

	// Make Match Data In Database
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		dbConn->Unbind();
		constexpr auto query = L"INSERT INTO [dbo].[MatchRecords] OUTPUT inserted.match_id DEFAULT VALUES;";

		WCHAR outMatchGUID[100];
		SQLLEN outMatchIDLen = 0;
		ASSERT_CRASH(dbConn->BindCol(1, outMatchGUID, 100, &outMatchIDLen));

		ASSERT_CRASH(dbConn->Execute(query));

		while (dbConn->Fetch());

		GDBConnectionPool->Push(dbConn);

		matchGUID_ = outMatchGUID;
		matchShortGUID_ = matchGUID_.substr(0, 8);
	}

	ASSERT_CRASH(isMatchInitialized_.exchange(true) == false);

	GLogHelper->Print(LogCategory::LOG_INFO, L"Match#%s Initialized\n", matchShortGUID_.c_str());
}


void Match::Start()
{
	GLogHelper->Print(LogCategory::LOG_INFO, L"Match#%s Start\n", matchShortGUID_.c_str());

	GTimerTaskManager->RemoveTimer(timeOutHandle_);

	matchStartTick_ = GetTickCount64();
	matchEndTick_ = matchStartTick_ + MATCH_END_TICK;
	matchStartTime_ = chrono::system_clock::now();

	ProjectJ::S_MATCH_START sendPacket;
	sendPacket.set_current_tick(matchStartTick_);
	sendPacket.set_end_tick(matchEndTick_);

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	Broadcast(sendBuffer);

	// TODO TICK
	tickHandle_ = GTimerTaskManager->AddTimer(15, true, this, &Match::Tick);

	ASSERT_CRASH(isMatchStarted_.exchange(true) == false);
}

void Match::End()
{
	if (isMatchEnded_.exchange(true) == true)
	{
		return;
	}

	GLogHelper->Print(LogCategory::LOG_INFO, L"Match#%s Ended\n", matchShortGUID_.c_str());

	GTimerTaskManager->RemoveTimer(tickHandle_);
	GTimerTaskManager->RemoveTimer(timeOutHandle_);

	vector<int> scores(MAX_PLAYER_NUMBER);
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		int weightScore = 0, itemScore = 0;
		int playScore = players_[i]->GetScore();

		if (players_[i]->GetState() == ProjectJ::ESCAPED)
		{
			weightScore = players_[i]->GetInventory()->GetCurrentWeight() * 10;
			itemScore = players_[i]->GetInventory()->GetItemsCount() * 100;
		}

		scores[i] = playScore + weightScore + itemScore;
	}

	if (isMatchStarted_.load() == true)
	{
		DBTimestampConverter timestampConverter;
		SQL_TIMESTAMP_STRUCT startTimestamp = timestampConverter(matchStartTime_);
		SQL_TIMESTAMP_STRUCT endTimestamp = timestampConverter();

		DBConnection* dbConn = GDBConnectionPool->Pop();

		// DB Match Records
		{
			dbConn->Unbind();
			constexpr auto matchRecordQuery = L"		\
						UPDATE [dbo].[MatchRecords]		\
						SET	start_time = ?,				\
							end_time = ?				\
						WHERE match_id = ?;";

			SQLLEN params[3]{0,};
			ASSERT_CRASH(dbConn->BindParam(1, &startTimestamp, &params[0]));
			ASSERT_CRASH(dbConn->BindParam(2, &endTimestamp, &params[1]));
			ASSERT_CRASH(dbConn->BindParam(3, matchGUID_.c_str(), &params[2]));

			ASSERT_CRASH(dbConn->Execute(matchRecordQuery));
		}

		// DB Match Players
		{
			dbConn->Unbind();

			constexpr auto matchPlayerQuery = L"																								\
							INSERT INTO [dbo].[MatchPlayers] (match_id, player_id, is_chaser, is_fugitive, kills, is_murdered, escaped, score)	\
							VALUES (?, ?, ?, ?, ?, ?, ?, ?), (?, ?, ?, ?, ?, ?, ?, ?), (?, ?, ?, ?, ?, ?, ?, ?), (?, ?, ?, ?, ?, ?, ?, ?);";


			vector<MatchRecords> params(4);
			constexpr int paramsCount = 8;

			for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
			{
				auto player = players_[i];

				params[i].playerID = player->GetID();
				params[i].isChaser = i == CHASER_INDEX ? true : false;
				params[i].isFugitive = !params[i].isChaser;
				params[i].kills = 0;
				params[i].isMurdered = i != CHASER_INDEX ? (player->GetState() == ProjectJ::MURDERED) : false;
				params[i].escaped = i != CHASER_INDEX ? (player->GetState() == ProjectJ::ESCAPED) : false;
				params[i].score = scores[i];

				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 1, matchGUID_.c_str(), &params[i].len[0]));
				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 2, &params[i].playerID, &params[i].len[1]));
				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 3, &params[i].isChaser, &params[i].len[2]));
				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 4, &params[i].isFugitive, &params[i].len[3]));
				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 5, &params[i].kills, &params[i].len[4]));
				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 6, &params[i].isMurdered, &params[i].len[5]));
				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 7, &params[i].escaped, &params[i].len[6]));
				ASSERT_CRASH(dbConn->BindParam(i * paramsCount + 8, &params[i].score, &params[i].len[7]));
			}

			dbConn->Execute(matchPlayerQuery);
		}


		GDBConnectionPool->Push(dbConn);
	}

	ProjectJ::S_MATCH_END sendPacket;

	sendPacket.set_chaser_score(scores[CHASER_INDEX]);
	sendPacket.set_fugitivie_first_score(scores[FUGITIVE_FIRST_INDEX]);
	sendPacket.set_fugitivie_first_score(scores[FUGITIVE_SECOND_INDEX]);
	sendPacket.set_fugitivie_first_score(scores[FUGITIVE_THIRD_INDEX]);

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	Broadcast(sendBuffer);

	PlayerBackToRoom();
	ownerRoom_->DoTaskAsync(&Room::EndMatch);
}

void Match::StartTimeOut()
{
	if (isMatchStarted_.load() == false)
	{
		GLogHelper->Print(LogCategory::LOG_INFO, L"Match#%s Start Time Out\n", matchShortGUID_.c_str());

		End();
	}
}

bool Match::CheckPlayersState()
{
	// Check Chaser Disconnected
	if (players_[CHASER_INDEX]->GetState() == ProjectJ::DISCONNECTED)
	{
		return true;
	}

	{
		// Check All Fugitives Disconnected
		bool flag = false;
		for (int i = FUGITIVE_FIRST_INDEX; i <= FUGITIVE_THIRD_INDEX; i++)
		{
			flag |= (players_[i]->GetState() == ProjectJ::DISCONNECTED ? false : true);
		}

		return flag == false;
	}
}

void Match::PlayerStateChanged(const shared_ptr<GameSession>& session, ProjectJ::MatchPlayerState state)
{
	for (auto player : players_)
	{
		if (player == session->GetPlayer())
		{
			player->SetState(state);
		}
	}
}

void Match::PlayerDisconnected(const shared_ptr<GameSession>& session)
{
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i] == session->GetPlayer())
		{
			players_[i]->SetState(ProjectJ::DISCONNECTED);
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
	if (isMatchStarted_.load() != false)
	{
		return;
	}

	auto player = session->GetPlayer();

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i] == player)
		{
			players_[i]->SetState(ProjectJ::LOADING);
		}
	}

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i]->GetState() != ProjectJ::LOADING)
		{
			return;
		}
	}

	int spinCount = 0;
	while (isMatchInitialized_.load() == false)
	{
		++spinCount;

		if (spinCount % 5000 == 0)
		{
			this_thread::yield();
		}
	}

	GLogHelper->Print(LogCategory::LOG_INFO, L"Match#%s Ready To Receive\n", matchShortGUID_.c_str());

	ProjectJ::S_MATCH_ALL_READY_TO_RECIEVE sendPacket;
	auto initInfo = new ProjectJ::MatchInitInfo();

	initInfo->set_allocated_chaser(players_[CHASER_INDEX]->GetPlayerInfo());
	initInfo->set_allocated_fugitive_first(players_[FUGITIVE_FIRST_INDEX]->GetPlayerInfo());
	initInfo->set_allocated_fugitive_second(players_[FUGITIVE_SECOND_INDEX]->GetPlayerInfo());
	initInfo->set_allocated_fugitive_third(players_[FUGITIVE_THIRD_INDEX]->GetPlayerInfo());

	// Scale first
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_FIRST_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_FIRST_INDEX]->GetTolerance());
		scaleInfo->set_inv_size_height(SCALE_HEIGHT);
		scaleInfo->set_inv_size_width(SCALE_WIDTH);
		initInfo->set_allocated_scale_first(scaleInfo);
	}

	// Scale second 
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_SECOND_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_SECOND_INDEX]->GetTolerance());
		scaleInfo->set_inv_size_height(SCALE_HEIGHT);
		scaleInfo->set_inv_size_width(SCALE_WIDTH);
		initInfo->set_allocated_scale_second(scaleInfo);
	}

	// Scale third
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_THIRD_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_THIRD_INDEX]->GetTolerance());
		scaleInfo->set_inv_size_height(SCALE_HEIGHT);
		scaleInfo->set_inv_size_width(SCALE_WIDTH);
		initInfo->set_allocated_scale_third(scaleInfo);
	}

	// Scale fourth
	{
		auto scaleInfo = new ProjectJ::MatchInitInfo_ScaleInitInfo();
		scaleInfo->set_operating_weight(scales_[SCALE_FOURTH_INDEX]->GetOperatingWeight());
		scaleInfo->set_tolerance(scales_[SCALE_FOURTH_INDEX]->GetTolerance());
		scaleInfo->set_inv_size_height(SCALE_HEIGHT);
		scaleInfo->set_inv_size_width(SCALE_WIDTH);
		initInfo->set_allocated_scale_fourth(scaleInfo);
	}

	sendPacket.set_allocated_info(initInfo);

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		sendPacket.set_your_player_index(i);
		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);

		players_[i]->GetOwnerSession()->Send(sendBuffer);
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
	if (isMatchStarted_.load() != false)
	{
		return;
	}

	auto player = session->GetPlayer();

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i] == player)
		{
			players_[i]->SetState(ProjectJ::ALIVE);
		}
	}

	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (players_[i]->GetState() != ProjectJ::ALIVE)
		{
			return;
		}
	}

	GLogHelper->Print(LogCategory::LOG_INFO, L"Match#%s Ready To Start\n", matchShortGUID_.c_str());

	Start();
}

void Match::PlayerSetTransform(const shared_ptr<GameSession>& session, int playerIndex, Vector&& position, Rotator&& rotation)
{
	if (playerIndex < 0 || playerIndex >= MAX_PLAYER_NUMBER || players_[playerIndex] != session->GetPlayer())
	{
		session->Disconnect();
		return;
	}

	players_[playerIndex]->SetTransform(move(position), move(rotation));
}

void Match::PlayerPickUpItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex)
{
	if (playerIndex < 0 || playerIndex >= MAX_PLAYER_NUMBER || itemIndex < 0 || itemIndex >= items_.size())
	{
		return;
	}

	shared_ptr<Player>& player = players_[playerIndex];
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
		                  L"Match#%s Player#%d Pick Up Item#%d Item ID: %d\n",
		                  matchShortGUID_.c_str(), playerIndex, itemIndex, item->id);

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
		                  L"Match#%s Player#%d Fail To Pick Up Item#%d: %s\n",
		                  matchShortGUID_.c_str(), playerIndex, itemIndex, Inventory::GetErrorWhat(errorCode));
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


	shared_ptr<Player>& player = players_[playerIndex];
	shared_ptr<Inventory> from = fromIndex < MAX_SCALE_NUMBER
		                             ? players_[fromIndex]->GetInventory()
		                             : scales_[fromIndex % MAX_SCALE_NUMBER]->GetInventory();
	shared_ptr<Inventory> to = toIndex < MAX_SCALE_NUMBER
		                           ? players_[toIndex]->GetInventory()
		                           : scales_[toIndex % MAX_SCALE_NUMBER]->GetInventory();
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

		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"Match#%s Player#%d Move Item#%d To Index#%d\n",
		                  matchShortGUID_.c_str(), playerIndex, itemIndex, toIndex);

		ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE sendPacket;

		sendPacket.set_from_index(fromIndex);
		sendPacket.set_to_index(toIndex);
		sendPacket.set_top_left_index(item->topLeftIndex);
		sendPacket.set_is_item_rotated(item->bIsRotated);
		sendPacket.set_item_index(itemIndex);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);

		if (fromIndex >= MAX_SCALE_NUMBER || fromIndex < MAX_INVENTORY_INDEX)
		{
			ProjectJ::S_MATCH_SCALE_ON_CHANGED scaleChangedPacket;
			shared_ptr<Scale> scale = static_pointer_cast<Scale>(from);

			scaleChangedPacket.set_scale_index(scale->GetIndex());
			scaleChangedPacket.set_is_operating(scale->IsOperating());
			scaleChangedPacket.set_current_weight(scale->GetCurrentWeight());

			auto scaleSendBuffer = GamePacketHandler::MakeSendBuffer(scaleChangedPacket);
			Broadcast(scaleSendBuffer);
		}

		if (toIndex >= MAX_SCALE_NUMBER || toIndex < MAX_INVENTORY_INDEX)
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
		                  L"Match#%s Player#%d Fail To Index#%d Move Item#%d: %s\n",
		                  matchShortGUID_.c_str(), playerIndex, toIndex, itemIndex, Inventory::GetErrorWhat(errorCode));
	}
}

void Match::PlayerDropItem(const shared_ptr<GameSession>& session, int playerIndex, int itemIndex, Vector position,
                           Rotator rotation)
{
	if (playerIndex < 0 || playerIndex >= MAX_PLAYER_NUMBER || itemIndex < 0 || itemIndex >= items_.size())
	{
		return;
	}

	shared_ptr<Player>& player = players_[playerIndex];
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

		GLogHelper->Print(LogCategory::LOG_WARN,
		                  L"Match#%s Player#%d Drop Item#%d\n",
		                  matchShortGUID_.c_str(), playerIndex, itemIndex);

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
		                  L"Match#%s Player#%d Fail To Drop Item#%d: %s\n",
		                  matchShortGUID_.c_str(), playerIndex, itemIndex, Inventory::GetErrorWhat(errorCode));
	}
}

void Match::ChaserAttack(const shared_ptr<GameSession>& session, const Vector& position, const Rotator& rotation)
{
	if (players_[CHASER_INDEX] != session->GetPlayer())
	{
		session->Disconnect();
	}

	ProjectJ::S_MATCH_CHASER_ATTACK sendPacket;
	auto sendPosition = new ProjectJ::Vector();
	auto sendRotation = new ProjectJ::Rotator();

	sendPosition->set_x(position.x_);
	sendPosition->set_y(position.y_);
	sendPosition->set_z(position.z_);

	sendRotation->set_roll(rotation.roll_);
	sendRotation->set_pitch(rotation.pitch_);
	sendRotation->set_yaw(rotation.yaw_);

	sendPacket.set_allocated_attack_position(sendPosition);
	sendPacket.set_allocated_attack_rotation(sendRotation);

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	Broadcast(sendBuffer);
}

void Match::HitValidation(const shared_ptr<GameSession>& session, const Vector& position, const Rotator& rotation, int targetPlayerIndex)
{
	if (targetPlayerIndex < 1 || targetPlayerIndex >= MAX_PLAYER_NUMBER || players_[CHASER_INDEX] != session->GetPlayer())
	{
		session->Disconnect();
		return;
	}

	Vector chaserPosition(position), targetPosition{0,};
	Rotator chaserRotation(rotation);

	targetPosition = players_[targetPlayerIndex]->GetVector();

	float distance = Vector::Distance(chaserPosition, targetPosition);

	Vector chaserVector2D = chaserRotation.Vector2D().Normalize2D();
	Vector toTargetVector2D = (targetPosition - chaserPosition).Normalize2D();

	float angle = Vector::Angle(chaserVector2D, toTargetVector2D);

	if (angle <= 90.0f && distance <= 100.0f)
	{
		auto hitPlayer = players_[targetPlayerIndex];
		ProjectJ::MatchPlayerState currentState = hitPlayer->GetState();
		ProjectJ::MatchPlayerState changedState = currentState;

		switch (currentState)
		{
		case ProjectJ::ALIVE:
			changedState = ProjectJ::ALIVE_DAMAGED;
			break;
		case ProjectJ::ALIVE_DAMAGED:
			changedState = ProjectJ::ALIVE_CRITICAL;
			break;
		case ProjectJ::ALIVE_CRITICAL:
			changedState = ProjectJ::ALIVE_MORIBUND;
			break;
		case ProjectJ::ALIVE_MORIBUND:
			changedState = ProjectJ::MURDERED;
			break;
		default:
			break;
		}

		hitPlayer->SetState(changedState);

		ProjectJ::S_MATCH_CHASER_HIT sendPacket;

		sendPacket.set_result(true);
		sendPacket.set_hit_player_index(targetPlayerIndex);
		sendPacket.set_changed_state(changedState);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
}


void Match::PlayerBackToRoom()
{
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (auto session = players_[i]->GetOwnerSession())
		{
			if (players_[i]->GetState() != ProjectJ::DISCONNECTED)
			{
				session->ProcessLeaveMatch();
			}

			players_[i]->SetState(ProjectJ::NONE);
		}
	}
}
