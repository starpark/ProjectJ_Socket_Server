#include "pch.h"
#include "GamePacketHandler.h"
#include "GameSession.h"
#include "Service.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(shared_ptr<GameSession>& session, BYTE* bufer, int numOfBytes)
{
	// TODO Log
	return false;
}

bool Handle_C_VERIFY_TOKEN(shared_ptr<GameSession>& session, ProjectJ::C_VERIFY_TOKEN& packet)
{
	wstring token = wstring().assign(packet.token().begin(), packet.token().end());
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"Handle_C_VERIFY_TOKEN: %d %s\n", packet.account_id(),
	                        token.c_str());
	return false;
}

bool Handle_C_LOBBY_CHAT(shared_ptr<GameSession>& session, ProjectJ::C_LOBBY_CHAT& packet)
{
	cout << packet.nickname() << ": " << packet.chat() << endl;

	ProjectJ::S_LOBBY_CHAT chatPacket{};
	chatPacket.set_account_id(packet.account_id());
	chatPacket.set_nickname(packet.nickname());
	chatPacket.set_chat_time(0);
	chatPacket.set_chat("Hi Client");
	auto sendBuffer = GamePacketHandler::MakeSendBuffer(chatPacket);
	session->Send(sendBuffer);


	return true;
}
