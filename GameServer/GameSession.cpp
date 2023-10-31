#include "pch.h"
#include "GameSession.h"
#include "GamePacketHandler.h"

// GameSessionMananger GSessionManager;

GameSession::GameSession()
{
	// 	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"GameSession()\n");
}

GameSession::~GameSession()
{
	player_ = nullptr;
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
	// GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnSend\n");
}


/*
void GameSessionMananger::Add(shared_ptr<GameSession> session)
{
	WRITE_LOCK;
	session->SetState(SessionState::LOBBY);
	sessions_.insert(session);
}

void GameSessionMananger::Remove(shared_ptr<GameSession> session)
{
	WRITE_LOCK;
	session->SetState(SessionState::NONE);
	sessions_.erase(session);
}

void GameSessionMananger::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;
	for (auto session : sessions_)
	{
		session->Send(sendBuffer);
	}
}

void GameSessionMananger::Broadcast(SessionState state, shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;
	for (auto session : sessions_)
	{
		if (session->GetState() == state)
		{
			session->Send(sendBuffer);
		}
	}
}
*/
