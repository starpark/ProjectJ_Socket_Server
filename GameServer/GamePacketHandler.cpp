#include "pch.h"
#include "GamePacketHandler.h"
#include "GameSession.h"
#include "GameService.h"
#include "Lobby.h"
#include "jwt/jwt.hpp"

PacketHandlerFunc GPacketHandler[UINT16_MAX];
static const string_view secretKey = "09d25e094faa6ca2556c818166b7a9563b93f7099f6f0f4caa6cf63b88e8d3e7";

bool Handle_INVALID(shared_ptr<GameSession>& session, BYTE* bufer, int numOfBytes)
{
	session->Disconnect();
	return false;
}

bool Handle_C_VERIFY_TOKEN(shared_ptr<GameSession>& session, ProjectJ::C_VERIFY_TOKEN& packet)
{
	using namespace jwt::params;

	error_code ec;
	auto decodeObj = jwt::decode(packet.token(), algorithms({"HS256"}), ec, secret(secretKey),
	                             issuer("ProjectJ API Server"), verify(true));


	ProjectJ::S_VERIFY_TOKEN verifyPacket;
	cout << packet.token() << endl;

	switch (ec.value())
	{
	case static_cast<int>(jwt::VerificationErrc::InvalidIssuer):
	case static_cast<int>(jwt::VerificationErrc::TokenExpired):
		verifyPacket.set_result(false);
		break;
	default:
		try
		{
			auto newPlayer = make_shared<Player>();

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
				newPlayer->id_ = decodeObj.payload().get_claim_value<int>("player_id");
			}
			else
			{
				throw;
			}

			if (auto service = static_pointer_cast<GameService>(session->GetService()))
			{
				session->SetLobby(service->GetLobby());
			}
			else
			{
				throw;
			}

			session->SetPlayer(newPlayer);
			session->SetState(SessionState::LOBBY);


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
	shared_ptr<Lobby> lobby = session->TryGetLobby();
	if (lobby)
	{
		cout << packet.nickname() << ": " << packet.chat() << endl;
		shared_ptr<Player> player = session->GetPlayer();

		if (player->nickname_ != packet.nickname() || player->id_ != packet.account_id())
		{
			session->Disconnect();
			return false;
		}

		ProjectJ::S_LOBBY_CHAT sendPacket;
		sendPacket.set_nickname(packet.nickname());
		sendPacket.set_account_id(packet.account_id());
		sendPacket.set_chat(packet.chat());

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		lobby->Broadcast(sendBuffer);
	}

	return true;
}

bool Handle_C_LOBBY_REFRESH_ROOM(shared_ptr<GameSession>& session, ProjectJ::C_LOBBY_REFRESH_ROOM& packet)
{
	shared_ptr<Lobby> lobby = session->TryGetLobby();
	if (lobby != nullptr)
	{
		// TEST CREATE ROOM
		lobby->CreateRoom(session, "规力格 1");
		lobby->CreateRoom(session, "规力格 2");
		lobby->CreateRoom(session, "规力格 3");

		auto roomList = lobby->GetRoomList();

		ProjectJ::S_LOBBY_REFRESH_ROOM sendPacket;

		for (auto room : roomList)
		{
			auto data = sendPacket.add_rooms();
			data->set_id(room->GetID());
			data->set_title(room->GetTitle());
			data->set_number_of_player(room->GetNumberOfPlayer());
		}

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		session->Send(sendBuffer);
	}

	return true;
}

bool Handle_C_LOBBY_CREATE_ROOM(shared_ptr<GameSession>& session, ProjectJ::C_LOBBY_CREATE_ROOM& packet)
{
	return true;
}

bool Handle_C_LOBBY_ENTER_ROOM(shared_ptr<GameSession>& session, ProjectJ::C_LOBBY_ENTER_ROOM& packet)
{
	return true;
}

bool Handle_C_ROOM_LEAVE(shared_ptr<GameSession>& session, ProjectJ::C_ROOM_LEAVE& packet)
{
	return true;
}

bool Handle_C_ROOM_READY(shared_ptr<GameSession>& session, ProjectJ::C_ROOM_READY& packet)
{
	return true;
}

bool Handle_C_MATCH_ITEM_PICKUP(shared_ptr<GameSession>& session, ProjectJ::C_MATCH_ITEM_PICKUP& packet)
{
	return true;
}

bool Handle_C_MATCH_ITEM_MOVE(shared_ptr<GameSession>& session, ProjectJ::C_MATCH_ITEM_MOVE& packet)
{
	return true;
}

bool Handle_C_MATCH_ITEM_DROP(shared_ptr<GameSession>& session, ProjectJ::C_MATCH_ITEM_DROP& packet)
{
	return true;
}
