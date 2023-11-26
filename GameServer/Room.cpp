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
	GLogHelper->Print(LogCategory::LOG_INFO, L"Room()#%d\n", roomID_);
	sessionSlots_.assign(4, make_pair(nullptr, false));
}

Room::~Room()
{
	GLogHelper->Print(LogCategory::LOG_INFO, L"~Room()#%d\n", roomID_);
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

int Room::GetSlotIndex(shared_ptr<GameSession> session)
{
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == session)
		{
			return i;
		}
	}

	return INVALID_SLOT_INDEX;
}

ProjectJ::RoomInfo* Room::MakeRoomInfo()
{
	auto roomInfo = new ProjectJ::RoomInfo();

	roomInfo->set_title(title_);
	roomInfo->set_room_id(roomID_);

	vector<pair<shared_ptr<GameSession>, bool>>& info = sessionSlots_;
	// 0: chaser
	{
		auto chaser = new ProjectJ::RoomInfo_PlayerSlot();

		if (info[0].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[0].first->GetID());
			player->set_nickname(info[0].first->GetNickname());
			chaser->set_allocated_player(player);
			chaser->set_is_ready(info[0].second);
		}
		else
		{
			chaser->clear_player();
			chaser->clear_is_ready();
		}

		roomInfo->set_allocated_chaser(chaser);
	}

	// 1: fugitive one
	{
		auto fugitive = new ProjectJ::RoomInfo_PlayerSlot();

		if (info[1].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[1].first->GetID());
			player->set_nickname(info[1].first->GetNickname());
			fugitive->set_allocated_player(player);
			fugitive->set_is_ready(info[1].second);
		}
		else
		{
			fugitive->clear_player();
			fugitive->clear_is_ready();
		}

		roomInfo->set_allocated_fugitive_first(fugitive);
	}

	// 2: fugitive two
	{
		auto fugitive = new ProjectJ::RoomInfo_PlayerSlot();

		if (info[2].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[2].first->GetID());
			player->set_nickname(info[2].first->GetNickname());
			fugitive->set_allocated_player(player);
			fugitive->set_is_ready(info[2].second);
		}
		else
		{
			fugitive->clear_player();
			fugitive->clear_is_ready();
		}

		roomInfo->set_allocated_fugitive_second(fugitive);
	}

	// 3: fugitive three
	{
		auto fugitive = new ProjectJ::RoomInfo_PlayerSlot();

		if (info[3].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[3].first->GetID());
			player->set_nickname(info[3].first->GetNickname());
			fugitive->set_allocated_player(player);
			fugitive->set_is_ready(info[3].second);
		}
		else
		{
			fugitive->clear_player();
			fugitive->clear_is_ready();
		}

		roomInfo->set_allocated_fugitive_third(fugitive);
	}

	return roomInfo;
}

int Room::Enter(shared_ptr<GameSession> session)
{
	if (state_ == RoomState::INVALID)
	{
		GLogHelper->Print(LogCategory::LOG_INFO, L"%s Fail To Enter Room#%d: Invalid Room\n", UTF8_TO_WCHAR(session->GetNickname().c_str()), roomID_);

		return INVALID_SLOT_INDEX;
	}
	if (numberOfPlayers_ == MAX_PLAYER_NUMBER)
	{
		GLogHelper->Print(LogCategory::LOG_INFO, L"%s Fail To Enter Room#%d: Room Is Full\n", UTF8_TO_WCHAR(session->GetNickname().c_str()), roomID_);

		return INVALID_SLOT_INDEX;
	}
	if (state_ != RoomState::WAITING)
	{
		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"%s Fail To Enter Room#%d: Room State Is Not Waiting\n",
		                  UTF8_TO_WCHAR(session->GetNickname().c_str()),
		                  roomID_);

		return INVALID_SLOT_INDEX;
	}


	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == nullptr)
		{
			GLogHelper->Print(LogCategory::LOG_INFO, L"%s Entered Room#%d\n", UTF8_TO_WCHAR(session->GetNickname().c_str()), roomID_);

			sessionSlots_[i].first = session;
			sessionSlots_[i].second = false;

			session->ProcessEnterRoom(GetRoomPtr());

			numberOfPlayers_++;

			{
				ProjectJ::S_ROOM_OTHER_ENTER sendPacket;

				sendPacket.set_room_id(roomID_);
				sendPacket.set_slot_index(i);
				sendPacket.set_allocated_other(session->MakePlayer());

				auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
				BroadcastWithoutSelf(sendBuffer, session);
			}

			return i;
		}
	}

	GLogHelper->Print(LogCategory::LOG_INFO, L"%s Fail To Enter Room#%d: Unexpected Error\n", UTF8_TO_WCHAR(session->GetNickname().c_str()), roomID_);

	return INVALID_SLOT_INDEX;
}

int Room::Leave(shared_ptr<GameSession> session, int slotIndex)
{
	if (slotIndex < 0 || slotIndex > MAX_PLAYER_NUMBER)
	{
		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"%s Fail To Leave Room#%d: Invalid Slot Index %d\n",
		                  UTF8_TO_WCHAR(session->GetNickname().c_str()),
		                  roomID_,
		                  slotIndex);

		return INVALID_SLOT_INDEX;
	}
	if (sessionSlots_[slotIndex].first != session)
	{
		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"%s Fail To Leave Room#%d: Session Doesn't Match The Slot Index#%d\n",
		                  UTF8_TO_WCHAR(session->GetNickname().c_str()),
		                  roomID_,
		                  slotIndex);

		return INVALID_SLOT_INDEX;
	}

	GLogHelper->Print(LogCategory::LOG_INFO, L"%s Leaved Room#%d\n", UTF8_TO_WCHAR(session->GetNickname().c_str()), roomID_);

	sessionSlots_[slotIndex].first = nullptr;
	sessionSlots_[slotIndex].second = false;
	numberOfPlayers_--;

	session->ProcessLeaveRoom();

	if (numberOfPlayers_ == 0)
	{
		state_ = RoomState::INVALID;

		if (auto lobby = GetLobby())
		{
			lobby->DoTaskAsync(&Lobby::DestroyRoom, roomID_);
		}
	}
	else
	{
		ProjectJ::S_ROOM_OTHER_LEAVE leavePacket;

		leavePacket.set_room_id(roomID_);
		leavePacket.set_slot_index(slotIndex);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(leavePacket);
		Broadcast(sendBuffer);
	}

	return slotIndex;
}

void Room::SessionReadyToReceive(shared_ptr<GameSession> session)
{
	GLogHelper->Print(LogCategory::LOG_INFO,
	                  L"%s Ready To Receive In Room#%d\n",
	                  UTF8_TO_WCHAR(session->GetNickname().c_str()),
	                  GetID());

	ProjectJ::S_ROOM_INFO sendPacket;

	sendPacket.set_allocated_info(MakeRoomInfo());

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	session->Send(sendBuffer);
}

void Room::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	for (auto session : sessionSlots_)
	{
		if (session.first == nullptr)
		{
			continue;
		}
		session.first->Send(sendBuffer);
	}
}

void Room::BroadcastWithoutSelf(shared_ptr<SendBuffer> sendBuffer, shared_ptr<GameSession> self)
{
	for (auto session : sessionSlots_)
	{
		if (session.first == self || session.first == nullptr)
		{
			continue;
		}
		session.first->Send(sendBuffer);
	}
}

bool Room::ChangePlayerPosition(shared_ptr<GameSession> session, int currentNumber, int desireNumber)
{
	if (currentNumber < 0 || currentNumber > 4 || desireNumber < 0 || desireNumber > 4)
	{
		return false;
	}

	if (sessionSlots_[desireNumber].first == nullptr && sessionSlots_[currentNumber].first == session)
	{
		sessionSlots_[currentNumber].second = false;
		sessionSlots_[desireNumber].second = false;

		swap(sessionSlots_[desireNumber].first, sessionSlots_[currentNumber].first);
	}

	return true;
}

void Room::ToggleReady(shared_ptr<GameSession> session, int slotIndex)
{
	if (slotIndex < 0 || slotIndex > MAX_PLAYER_NUMBER || sessionSlots_[slotIndex].first != session)
	{
		GLogHelper->Print(LogCategory::LOG_INFO, L"%s Try To Invalid Slot Index#%d In Room#%d\n",
		                  UTF8_TO_WCHAR(session->GetNickname().c_str()),
		                  slotIndex,
		                  roomID_);

		return;
	}

	sessionSlots_[slotIndex].second ^= true;

	GLogHelper->Print(LogCategory::LOG_INFO, L"%s Slot#%d %s In Room#%d\n",
	                  UTF8_TO_WCHAR(session->GetNickname().c_str()),
	                  slotIndex,
	                  (sessionSlots_[slotIndex].second == true ? L"Ready" : L"Unready"),
	                  roomID_);

	ProjectJ::S_ROOM_READY sendPacket;

	sendPacket.set_slot_index(slotIndex);
	sendPacket.set_is_ready(sessionSlots_[slotIndex].second);

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	Broadcast(sendBuffer);

	StandByMatch(STANDBY_COUNT);
}

bool Room::CheckAllReady()
{
	for (int i = 0; i < MAX_PLAYER_NUMBER; i++)
	{
		if (sessionSlots_[i].first == nullptr || sessionSlots_[i].second == false)
		{
			return false;
		}
	}

	return true;
}

void Room::StandByMatch(int count)
{
	if (CheckAllReady())
	{
		{
			ProjectJ::S_ROOM_STANDBY_MATCH sendPacket;
			sendPacket.set_count(count);

			auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
			Broadcast(sendBuffer);
		}

		if (count == 0)
		{
			StartMatch();
		}
		else
		{
			GTimerTaskManager->AddTimer(1000, false, this, &Room::StandByMatch, count - 1);
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

	match_ = make_shared<Match>(GetRoomPtr());
	match_->DoTaskAsync(&Match::Init, chaser, fugitiveFirst, fugitiveSecond, fugitiveThird);
	{
		ProjectJ::S_ROOM_START_MATCH sendPacket;
		sendPacket.set_start(true);

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		Broadcast(sendBuffer);
	}
}

void Room::EndMatch()
{
	state_ = RoomState::WAITING;
	for (auto& session : sessionSlots_)
	{
		session.second = false;
	}

	match_ = nullptr;
}

void Room::DestroyMatch()
{
	match_->End();
	match_ = nullptr;
}
