#include "pch.h"
#include "GameSession.h"
#include "GamePacketHandler.h"

GameSession::GameSession()
	: state_(SessionState::NONE)
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"GameSession()\n");
}

GameSession::~GameSession()
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"~GameSession()\n");
}

void GameSession::ChangeState(SessionState state)
{
	state_ = state;
}

void GameSession::OnConnected()
{
	//GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnConnected\n");
}

void GameSession::OnDisconnect()
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnDisconnect\n");
}

int GameSession::OnRecv(BYTE* buffer, int numOfBytes)
{
	//GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnRecv\n");
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

		shared_ptr<GameSession> Session = GetGameSessionPtr();
		GamePacketHandler::HandlePacket(Session, buffer, header.size);

		processLen += header.size;
	}

	return processLen;
}

void GameSession::OnSend(int numOfBytes)
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnSend\n");
}
