#include "pch.h"
#include "GameSession.h"
#include "GamePacketHandler.h"
#include "Lobby.h"
#include "Match.h"
#include "Room.h"

GameSession::GameSession()
{
	// 	GLogHelper->WriteLog(LogCategory::Log_TEMP, L"GameSession()\n");
}

GameSession::~GameSession()
{
	player_ = nullptr;

	GLogHelper->Reserve(LogCategory::Log_INFO, L"~GameSession()\n");
}

void GameSession::ProcessEnterLobby(const shared_ptr<Lobby>& lobby)
{
	SetState(SessionState::LOBBY);
	SetLobby(lobby);
}

void GameSession::ProcessLeaveLobby()
{
	SetState(SessionState::NONE);
	lobby_.reset();
}

void GameSession::ProcessEnterRoom(const shared_ptr<Room>& room)
{
	SetState(SessionState::ROOM);
	SetRoom(room);
}

void GameSession::ProcessLeaveRoom()
{
	SetState(SessionState::LOBBY);
	room_.reset();
}

void GameSession::ProcessEnterMatch(const shared_ptr<Match>& match, const shared_ptr<Player>& player)
{
	SetState(SessionState::MATCH);
	player_ = player;
	SetMatch(match);
}

void GameSession::ProcessLeaveMatch()
{
	SetState(SessionState::ROOM);
	player_ = nullptr;
	match_.reset();
}

void GameSession::OnConnected()
{
	// GLogHelper->Reserve(LogCategory::Log_INFO, L"OnConnected %s\n", netAddress_.GetIpAddressW().c_str());
}

void GameSession::OnDisconnect()
{
	GLogHelper->Reserve(LogCategory::Log_INFO, L"OnDisconnect %s\n", netAddress_.GetIpAddressW().c_str());

	if (auto lobby = lobby_.lock())
	{
		if (auto room = room_.lock())
		{
			shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(shared_from_this());
			lobby->LeaveRoom(gameSession, room->GetID());
			room_.reset();
		}
		lobby_.reset();
	}
}

int GameSession::OnRecv(BYTE* buffer, int numOfBytes)
{
	// GLogHelper->Reserve(LogCategory::Log_TEMP, L"OnRecv\n");

	int processLen = 0;
	while (true)
	{
		int dataSize = numOfBytes - processLen;
		if (dataSize <= sizeof(PacketHeader))
		{
			break;
		}

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		if (dataSize < header.size)
		{
			break;
		}

		shared_ptr<SessionBase> session = GetSessionPtr();
		GamePacketHandler::HandlePacket(session, buffer, header.size);

		processLen += header.size;
	}

	return processLen;
}

void GameSession::OnSend(int numOfBytes)
{
	// GLogHelper->WriteLog(LogCategory::Log_TEMP, L"OnSend\n");
}
