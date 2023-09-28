#include "pch.h"
#include "GamePacketHandler.h"

#include "GameSession.h"
#include "Service.h"

/*
 *	Handler
 */

bool Handle_Invalid(shared_ptr<GameSession>& session, BYTE* bufer, int numOfBytes)
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


PacketHandler GamePacketHandler::handlers_[HANDLER_SIZE];
