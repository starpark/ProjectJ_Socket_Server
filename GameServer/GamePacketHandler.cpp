#include "pch.h"
#include "GamePacketHandler.h"
#include "GameSession.h"
#include "GameService.h"
#include "Lobby.h"
#include "jwt/jwt.hpp"

PacketHandlerFunc GPacketHandler[UINT16_MAX];
static constexpr string_view secretKey = "09d25e094faa6ca2556c818166b7a9563b93f7099f6f0f4caa6cf63b88e8d3e7";

/* HELPER FUNCTIONS */

ProjectJ::RoomInfo* MakeRoomInfo(shared_ptr<Room> room)
{
	auto roomInfo = new ProjectJ::RoomInfo();

	roomInfo->set_title(room->GetTitle());
	roomInfo->set_room_id(room->GetID());

	vector<pair<shared_ptr<GameSession>, bool>> info = room->GetRoomInfo();
	// 0: chaser
	{
		auto chaser = new ProjectJ::RoomInfo_PlayerSlot();
		chaser->set_is_ready(info[0].second);

		if (info[0].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[0].first->GetID());
			player->set_nickname(info[0].first->GetNickname());
			chaser->set_allocated_player(player);
		}

		roomInfo->set_allocated_chaser(chaser);
	}

	// 1: fugitive one
	{
		auto fugitive = new ProjectJ::RoomInfo_PlayerSlot();
		fugitive->set_is_ready(info[1].second);

		if (info[1].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[1].first->GetID());
			player->set_nickname(info[1].first->GetNickname());
			fugitive->set_allocated_player(player);
		}

		roomInfo->set_allocated_fugitive_first(fugitive);
	}

	// 2: fugitive two
	{
		auto fugitive = new ProjectJ::RoomInfo_PlayerSlot();
		fugitive->set_is_ready(info[2].second);

		if (info[2].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[2].first->GetID());
			player->set_nickname(info[2].first->GetNickname());
			fugitive->set_allocated_player(player);
		}

		roomInfo->set_allocated_fugitive_second(fugitive);
	}

	// 3: fugitive three
	{
		auto fugitive = new ProjectJ::RoomInfo_PlayerSlot();
		fugitive->set_is_ready(info[3].second);

		if (info[3].first != nullptr)
		{
			auto player = new ProjectJ::Player();
			player->set_account_id(info[3].first->GetID());
			player->set_nickname(info[3].first->GetNickname());
			fugitive->set_allocated_player(player);
		}

		roomInfo->set_allocated_fugitive_third(fugitive);
	}

	return roomInfo;
}

ProjectJ::Player* MakePlayer(shared_ptr<GameSession> session)
{
	if (session->IsVerified() == false)
	{
		return nullptr;
	}

	auto player = new ProjectJ::Player();
	player->set_account_id(session->GetID());
	player->set_nickname(session->GetNickname());
	return player;
}

/* LOBBY ROOM PROTOCOLS */

bool Handle_INVALID(shared_ptr<SessionBase>& session, BYTE* bufer, int numOfBytes)
{
	session->Disconnect();
	return false;
}

bool Handle_C_VERIFY_TOKEN(shared_ptr<SessionBase>& session, ProjectJ::C_VERIFY_TOKEN& packet)
{
	using namespace jwt::params;

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	error_code ec;
	auto decodeObj = jwt::decode(packet.token(), algorithms({"HS256"}), ec, secret(secretKey),
	                             issuer("ProjectJ API Server"), verify(true));

#ifdef _DEBUG
	GLogHelper->Reserve(LogCategory::Log_INFO, "Authentication attempts: %s\n",
	                    gameSession->GetNetAddress().GetIpAddress().c_str());
#endif

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
			if (decodeObj.payload().has_claim("name") == true)
			{
				gameSession->SetName(std::move(decodeObj.payload().get_claim_value<string>("name")));
			}
			else
			{
				throw 1;
			}

			if (decodeObj.payload().has_claim("nickname") == true)
			{
				gameSession->SetNickname(std::move(decodeObj.payload().get_claim_value<string>("nickname")));
			}
			else
			{
				throw 2;
			}

			if (decodeObj.payload().has_claim("player_id") == true)
			{
				gameSession->SetID(decodeObj.payload().get_claim_value<int>("player_id"));
			}
			else
			{
				throw 3;
			}

			if (auto service = gameSession->GetService())
			{
				auto gameService = static_pointer_cast<GameService>(service);
				gameSession->ProcessEnterLobby(gameService->GetLobby());
			}
			else
			{
				throw 4;
			}

			GLogHelper->Reserve(LogCategory::Log_SUCCESS, "Authentication Success: %s %s\n",
			                    gameSession->GetNetAddress().GetIpAddress().c_str(),
			                    gameSession->GetNickname().c_str());
			verifyPacket.set_result(true);
			gameSession->SetIsVerified(true);
		}
		catch (nlohmann::json::exception& e)
		{
			// parsing type error
			GLogHelper->Reserve(LogCategory::Log_WARN, "Authentication Failure: %s\n", e.what());
			verifyPacket.set_result(false);
		}
		catch (int e)
		{
			GLogHelper->Reserve(LogCategory::Log_WARN, "Authentication Failure: Error Number %d\n", e);
			verifyPacket.set_result(false);
		}
		catch (...)
		{
			GLogHelper->Reserve(LogCategory::Log_WARN, "Authentication Failure: Unexpected Error\n");
			verifyPacket.set_result(false);
		}
		break;
	}

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(verifyPacket);
	gameSession->Send(sendBuffer);

	if (verifyPacket.result() == false)
	{
		gameSession->Disconnect();
	}

	return true;
}

bool Handle_C_LOBBY_CHAT(shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_CHAT& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();
	if (lobby)
	{
		ProjectJ::S_LOBBY_CHAT sendPacket;
		sendPacket.set_nickname(gameSession->GetNickname());
		sendPacket.set_account_id(gameSession->GetID());
		sendPacket.set_chat(packet.chat());

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		lobby->Broadcast(sendBuffer);
	}
	else
	{
		gameSession->Disconnect();
		return false;
	}

	return true;
}

bool Handle_C_LOBBY_REFRESH_ROOM(shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_REFRESH_ROOM& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();

	if (lobby)
	{
		GLogHelper->Reserve(LogCategory::Log_INFO, "Session %d %s Refresh Room\n", gameSession->GetID(),
		                    gameSession->GetNickname().c_str());

		ProjectJ::S_LOBBY_REFRESH_ROOM sendPacket;
		auto roomList = lobby->GetRoomList();

		for (auto room : roomList)
		{
			auto roomData = sendPacket.add_rooms();
			roomData->set_id(room->GetID());
			roomData->set_title(room->GetTitle());
			roomData->set_state(room->GetState() == RoomState::WAITING
				                    ? ProjectJ::RoomState::WAITING
				                    : ProjectJ::RoomState::INGAME);
			roomData->set_number_of_player(room->GetNumberOfPlayer());
		}

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		gameSession->Send(sendBuffer);
	}

	return true;
}

bool Handle_C_LOBBY_CREATE_ROOM(shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_CREATE_ROOM& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();
	ProjectJ::S_LOBBY_CREATE_ROOM sendPacket;

	if (lobby)
	{
		shared_ptr<Room> room = lobby->CreateRoom(gameSession, packet.title());
		if (room)
		{
			sendPacket.set_result(true);

			auto roomInfo = MakeRoomInfo(room);

			sendPacket.set_allocated_info(roomInfo);
		}
		else
		{
			sendPacket.set_result(false);
			sendPacket.clear_info();
		}
	}
	else
	{
		sendPacket.set_result(false);
		sendPacket.clear_info();
	}

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_LOBBY_ENTER_ROOM(shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_ENTER_ROOM& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();
	ProjectJ::S_LOBBY_ENTER_ROOM sendPacket;

	if (lobby)
	{
		if (auto room = lobby->EnterRoom(gameSession, packet.room_id()))
		{
			GLogHelper->Reserve(LogCategory::Log_INFO, "Session %d %s Enter Room %d\n", gameSession->GetID(),
			                    gameSession->GetNickname().c_str(), room->GetID());
			sendPacket.set_result(true);
			sendPacket.set_room_id(room->GetID());
			sendPacket.set_allocated_info(MakeRoomInfo(room));

			{
				ProjectJ::S_ROOM_OTHER_ENTER enterPacket;
				enterPacket.set_allocated_other(MakePlayer(gameSession));
				enterPacket.set_allocated_info(MakeRoomInfo(room));

				auto sendBuffer = GamePacketHandler::MakeSendBuffer(enterPacket);
				room->BroadcastWithoutSelf(sendBuffer, gameSession);
			}
		}
		else
		{
			sendPacket.set_result(false);
			sendPacket.clear_info();
		}
	}
	else
	{
		sendPacket.set_result(false);
		sendPacket.clear_info();
	}

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ROOM_LEAVE(shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_LEAVE& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Room> room = gameSession->TryGetRoom();
	ProjectJ::S_ROOM_LEAVE sendPacket;

	if (room)
	{
		auto lobby = room->GetLobby();
		auto slotIdx = lobby->LeaveRoom(gameSession, room->GetID());
		if (slotIdx != INVALID_ROOM_SLOT)
		{
			sendPacket.set_result(true);

			{
				ProjectJ::S_ROOM_OTHER_LEAVE leavePacket;
				leavePacket.set_allocated_info(MakeRoomInfo(room));
				leavePacket.set_allocated_other(MakePlayer(gameSession));

				auto sendBuffer = GamePacketHandler::MakeSendBuffer(leavePacket);
				room->BroadcastHere(sendBuffer);
			}
		}
		else
		{
			sendPacket.set_result(false);
		}
	}
	else
	{
		sendPacket.set_result(false);
	}

	auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ROOM_READY(shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_READY& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Room> room = gameSession->TryGetRoom();

	if (room)
	{
		ProjectJ::S_ROOM_READY sendPacket;
		room->ToggleReady(gameSession);

		sendPacket.set_allocated_info(MakeRoomInfo(room));
		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		room->BroadcastHere(sendBuffer);

		if (room->CheckAllReady())
		{
			room->StandByMatch(5);
		}
	}

	return true;
}

bool Handle_C_ROOM_CHAT(shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_CHAT& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	if (auto room = gameSession->TryGetRoom())
	{
		ProjectJ::S_ROOM_CHAT sendPacket;
		sendPacket.set_account_id(gameSession->GetID());
		sendPacket.set_nickname(gameSession->GetNickname());
		sendPacket.set_room_id(room->GetID());
		sendPacket.set_chat(packet.chat());

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		room->BroadcastHere(sendBuffer);
	}

	return true;
}

/* MATCH PROTOCOLS */

bool Handle_C_MATCH_LOADING_COMPLETE(shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_LOADING_COMPLETE& packet)
{
	return true;
}

bool Handle_C_MATCH_ITEM_PICKUP(shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_PICKUP& packet)
{
	return true;
}

bool Handle_C_MATCH_ITEM_MOVE(shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_MOVE& packet)
{
	return true;
}

bool Handle_C_MATCH_ITEM_DROP(shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_DROP& packet)
{
	return true;
}
