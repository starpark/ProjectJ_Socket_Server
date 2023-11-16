#include "pch.h"
#include "GameSession.h"
#include "GamePacketHandler.h"
#include "Lobby.h"
#include "Match.h"
#include "Room.h"

GameSession::GameSession()
{
}

GameSession::~GameSession()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"~GameSession()\n");

	player_.reset();
	lobby_.reset();
	room_.reset();
	match_.reset();
}

void GameSession::ProcessEnterLobby(const shared_ptr<Lobby>& lobby)
{
	WRITE_LOCK;
	SetState(SessionState::LOBBY);
	SetLobby(lobby);
}

void GameSession::ProcessLeaveLobby()
{
	WRITE_LOCK;
	SetState(SessionState::NONE);
	lobby_.reset();
}

void GameSession::ProcessEnterRoom(const shared_ptr<Room>& room)
{
	WRITE_LOCK;
	SetState(SessionState::ROOM);
	SetRoom(room);
}

void GameSession::ProcessLeaveRoom()
{
	WRITE_LOCK;
	SetState(SessionState::LOBBY);
	room_.reset();
}

void GameSession::ProcessEnterMatch(const shared_ptr<Match>& match, const shared_ptr<Player>& player)
{
	WRITE_LOCK;
	SetState(SessionState::MATCH);
	player_ = player;
	SetMatch(match);
}

void GameSession::ProcessLeaveMatch()
{
	WRITE_LOCK;
	SetState(SessionState::ROOM);
	player_ = nullptr;
	match_.reset();
}

ProjectJ::Player* GameSession::MakePlayer()
{
	if (IsVerified() == false)
	{
		return nullptr;
	}

	auto player = new ProjectJ::Player();
	player->set_account_id(GetID());
	player->set_nickname(GetNickname());
	return player;
}

void GameSession::OnConnected()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"OnConnected %s:%hu\n", netAddress_.GetIpAddressW().c_str(), netAddress_.GetPort());
}

void GameSession::OnDisconnect()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"OnDisconnect %s:%hu\n", netAddress_.GetIpAddressW().c_str(), netAddress_.GetPort());
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(shared_from_this());

	// Clean up used contents

	if (auto match = match_.lock())
	{
		match->PlayerDisconnected(gameSession);
		match_.reset();
	}

	if (auto lobby = lobby_.lock())
	{
		if (auto room = room_.lock())
		{
			room->DoTaskCallback(&Room::GetSlotIndex, [room, gameSession](int slotIndex)
			{
				if (slotIndex != INVALID_SLOT_INDEX && slotIndex < MAX_PLAYER_NUMBER)
				{
					room->Leave(gameSession, slotIndex);
				}
			}, gameSession);
		}
		ProcessLeaveLobby();
	}
}

int GameSession::OnRecv(BYTE* buffer, int numOfBytes)
{
	int processLen = 0;
	while (true)
	{
		int dataSize = numOfBytes - processLen;
		if (dataSize < sizeof(PacketHeader))
		{
			break;
		}

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		if (dataSize < header.size)
		{
			break;
		}

		shared_ptr<SessionBase> session = GetSessionPtr();
		GamePacketHandler::HandlePacket(session, &buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}

void GameSession::OnSend(int numOfBytes)
{
}
