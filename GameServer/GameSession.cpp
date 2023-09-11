#include "pch.h"
#include "GameSession.h"
#include "GamePacketHandler.h"

GameSession::GameSession()
	: state_(SessionState::WAIT_CONNECTING)
{
	// cout << "GameSession()" << endl;
}

GameSession::~GameSession()
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"~GameSession()\n");
}

void GameSession::OnConnected()
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnConnected\n");
}

void GameSession::OnDisconnect()
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnDisconnect\n");
}

int GameSession::OnRecv(BYTE* buffer, int numOfBytes)
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnRecv\n");
	int processLen = 0;

	while (true)
	{
		int dataSize = numOfBytes - processLen;
		if (dataSize <= PacketHeaderSize)
		{
			break;
		}

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		if (dataSize < header.size)
		{
			break;
		}

		GamePacketHandler::HandlePacket(GetGameSessionPtr(), buffer, numOfBytes);

		processLen += header.size;
	}

	return processLen;
}

void GameSession::OnSend(int numOfBytes)
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnSend\n");
}
