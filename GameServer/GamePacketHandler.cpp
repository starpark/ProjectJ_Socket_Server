#include "pch.h"
#include "GamePacketHandler.h"
#include "GameSession.h"
#include "Service.h"
#include "jwt/jwt.hpp"

PacketHandlerFunc GPacketHandler[UINT16_MAX];
static const string_view secretKey = "09d25e094faa6ca2556c818166b7a9563b93f7099f6f0f4caa6cf63b88e8d3e7";

bool Handle_INVALID(shared_ptr<GameSession>& session, BYTE* bufer, int numOfBytes)
{
	// TODO Log
	return false;
}

bool Handle_C_VERIFY_TOKEN(shared_ptr<GameSession>& session, ProjectJ::C_VERIFY_TOKEN& packet)
{
	using namespace jwt::params;

	error_code ec;
	auto decodeObj = jwt::decode(packet.token(), algorithms({"HS256"}), ec, secret(secretKey),
	                             issuer("ProjectJ API Server"), verify(true));

	ProjectJ::S_VERIFY_TOKEN verifyPacket;

	switch (ec.value())
	{
	case static_cast<int>(jwt::VerificationErrc::InvalidIssuer):
	case static_cast<int>(jwt::VerificationErrc::TokenExpired):
		verifyPacket.set_result(false);
		break;
	default:
		try
		{
			shared_ptr<Player> newPlayer = make_shared<Player>();

			newPlayer->ownerSession_ = session;
			if (decodeObj.payload().has_claim("name") == true)
			{
				newPlayer->name_ = decodeObj.payload().get_claim_value<string>("name");
			}
			else
			{
				throw;
			}

			if (decodeObj.payload().has_claim("nickname") == true)
			{
				newPlayer->nickname_ = decodeObj.payload().get_claim_value<string>("nickname");
			}
			else
			{
				throw;
			}

			if (decodeObj.payload().has_claim("player_id") == true)
			{
				newPlayer->playerId_ = decodeObj.payload().get_claim_value<int>("player_id");
			}
			else
			{
				throw;
			}

			session->SetPlayer(newPlayer);
			verifyPacket.set_result(true);
		}
		catch (nlohmann::json::exception& e)
		{
			// parsing type error
			GLogHelper->WriteStdOut(LogCategory::Log_ERROR, "%s\n", e.what());
			verifyPacket.set_result(false);
		}
		catch (...)
		{
			// claim not found
			verifyPacket.set_result(false);
		}
		break;
	}


	auto sendBuffer = GamePacketHandler::MakeSendBuffer(verifyPacket);
	session->Send(sendBuffer);

	if (verifyPacket.result() == false)
	{
		session->Disconnect();
	}

	return true;
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
