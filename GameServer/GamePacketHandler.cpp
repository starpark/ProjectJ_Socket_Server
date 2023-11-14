#include "pch.h"
#include "GamePacketHandler.h"
#include "GameSession.h"
#include "GameService.h"
#include "Lobby.h"
#include "Match.h"
#include "jwt/jwt.hpp"

PacketHandlerFunc GPacketHandler[UINT16_MAX];
static constexpr string_view secretKey = "09d25e094faa6ca2556c818166b7a9563b93f7099f6f0f4caa6cf63b88e8d3e7";

/* LOBBY ROOM PROTOCOLS */

bool Handle_INVALID(const shared_ptr<SessionBase>& session, BYTE* bufer, int numOfBytes)
{
	session->Disconnect();
	return false;
}

bool Handle_C_VERIFY_TOKEN(const shared_ptr<SessionBase>& session, ProjectJ::C_VERIFY_TOKEN& packet)
{
	using namespace jwt::params;

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	error_code ec;
	auto decodeObj = jwt::decode(packet.token(), algorithms({"HS256"}), ec, secret(secretKey),
	                             issuer("ProjectJ API Server"), verify(true));

#ifdef _DEBUG
	GLogHelper->Print(LogCategory::Log_INFO,
	                  L"Authentication attempts: %s\n",
	                  UTF8_TO_WCHAR(gameSession->GetNetAddress().GetIpAddress().c_str()));
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

			GLogHelper->Print(LogCategory::Log_SUCCESS, L"Authentication Success: %s %s\n",
			                  UTF8_TO_WCHAR(gameSession->GetNetAddress().GetIpAddress().c_str()),
			                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()));


			verifyPacket.set_result(true);
			gameSession->SetIsVerified(true);
		}
		catch (nlohmann::json::exception& e)
		{
			// parsing type error
			GLogHelper->Print(LogCategory::Log_WARN, L"Authentication Failure: %s\n", UTF8_TO_WCHAR(e.what()));
			verifyPacket.set_result(false);
		}
		catch (int e)
		{
			GLogHelper->Print(LogCategory::Log_WARN, L"Authentication Failure: Error Number %d\n", e);
			verifyPacket.set_result(false);
		}
		catch (...)
		{
			GLogHelper->Print(LogCategory::Log_WARN, L"Authentication Failure: Unexpected Error\n");
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

bool Handle_C_LOBBY_CHAT(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_CHAT& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();

	if (!gameSession && gameSession->IsVerified() == false)
	{
		gameSession->Disconnect();
		return false;
	}

	if (lobby)
	{
		GLogHelper->Print(LogCategory::Log_INFO,
		                  L"%s Chatted in Lobby: %s\n",
		                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()),
		                  UTF8_TO_WCHAR(packet.chat().c_str()));

		lobby->DoTaskAsync([&lobby, &gameSession, &packet]()
		{
			ProjectJ::S_LOBBY_CHAT sendPacket;
			sendPacket.set_nickname(gameSession->GetNickname());
			sendPacket.set_account_id(gameSession->GetID());
			sendPacket.set_chat(packet.chat());

			auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
			lobby->Broadcast(sendBuffer);
		});
	}
	else
	{
		gameSession->Disconnect();
		return false;
	}

	return true;
}

bool Handle_C_LOBBY_REFRESH_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_REFRESH_ROOM& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();

	if (!gameSession && gameSession->IsVerified() == false)
	{
		gameSession->Disconnect();
		return false;
	}

	if (lobby)
	{
		GLogHelper->Print(LogCategory::Log_INFO,
		                  L"%s#%d Has Refreshed Room List\n",
		                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()), gameSession->GetID());

		lobby->DoTaskCallback(&Lobby::GetRoomList, [gameSession](vector<shared_ptr<Room>> rooms)
		{
			ProjectJ::S_LOBBY_REFRESH_ROOM sendPacket;
			for (auto room : rooms)
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
		});
	}

	return true;
}

bool Handle_C_LOBBY_CREATE_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_CREATE_ROOM& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();

	if (!gameSession && gameSession->IsVerified() == false)
	{
		gameSession->Disconnect();
		return false;
	}

	if (lobby)
	{
		lobby->DoTaskCallback(&Lobby::CreateRoom, [gameSession](shared_ptr<Room> newRoom)
		{
			if (newRoom)
			{
				newRoom->DoTaskCallback(&Room::Enter, [newRoom, gameSession](bool success)
				{
					if (success)
					{
						ProjectJ::S_LOBBY_CREATE_ROOM sendPacket;
						sendPacket.set_result(true);
						sendPacket.set_allocated_info(newRoom->MakeRoomInfo());

						auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
						gameSession->Send(sendBuffer);
					}
				}, gameSession);
			}
			else
			{
				ProjectJ::S_LOBBY_CREATE_ROOM sendPacket;
				sendPacket.set_result(false);
				sendPacket.clear_info();

				auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
				gameSession->Send(sendBuffer);
			}
		}, gameSession, packet.title());
	}
	else
	{
		ProjectJ::S_LOBBY_CREATE_ROOM sendPacket;
		sendPacket.set_result(false);
		sendPacket.clear_info();

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		session->Send(sendBuffer);
		return false;
	}

	return true;
}

bool Handle_C_LOBBY_ENTER_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_ENTER_ROOM& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Lobby> lobby = gameSession->TryGetLobby();

	if (!gameSession && gameSession->IsVerified() == false)
	{
		gameSession->Disconnect();
		return false;
	}

	if (lobby)
	{
		lobby->DoTaskCallback(&Lobby::FindRoom, [gameSession](shared_ptr<Room> room)
		{
			if (room)
			{
				room->DoTaskCallback(&Room::Enter, [room, gameSession](bool success)
				{
					ProjectJ::S_LOBBY_ENTER_ROOM sendPacket;

					if (success)
					{
						sendPacket.set_result(true);
						sendPacket.set_allocated_info(room->MakeRoomInfo());
						sendPacket.set_room_id(room->GetID());
					}
					else
					{
						sendPacket.set_result(false);
						sendPacket.clear_info();
						sendPacket.clear_room_id();
					}

					auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
					gameSession->Send(sendBuffer);
				}, gameSession);
			}
			else
			{
				GLogHelper->Print(LogCategory::Log_INFO,
				                  L"%s Accessed an Invalid Room.\n",
				                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()));

				ProjectJ::S_LOBBY_ENTER_ROOM sendPacket;
				sendPacket.set_result(false);
				sendPacket.clear_info();

				auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
				gameSession->Send(sendBuffer);
			}
		}, packet.room_id());
	}
	else
	{
		gameSession->Disconnect();
		return false;
	}

	return true;
}

bool Handle_C_ROOM_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_LEAVE& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Room> room = gameSession->TryGetRoom();

	if (!gameSession && gameSession->IsVerified() == false)
	{
		gameSession->Disconnect();
		return false;
	}

	if (room)
	{
		if (room->GetID() != packet.room_id())
		{
			session->Disconnect();
			return false;
		}

		room->DoTaskCallback(&Room::Leave, [gameSession](int index)
		{
			ProjectJ::S_ROOM_LEAVE sendPacket;
			sendPacket.set_result(index > INVALID_ROOM_SLOT ? true : false);

			auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
			gameSession->Send(sendBuffer);
		}, gameSession);
	}
	else
	{
		session->Disconnect();
		return false;
	}

	return true;
}

bool Handle_C_ROOM_READY(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_READY& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Room> room = gameSession->TryGetRoom();

	if (!gameSession && gameSession->IsVerified() == false)
	{
		gameSession->Disconnect();
		return false;
	}

	if (room)
	{
		room->DoTaskAsync(&Room::ToggleReady, gameSession);
	}

	return true;
}

bool Handle_C_ROOM_CHAT(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_CHAT& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto room = gameSession->TryGetRoom();

	if (!gameSession && gameSession->IsVerified() == false)
	{
		gameSession->Disconnect();
		return false;
	}

	if (room)
	{
		GLogHelper->Print(LogCategory::Log_INFO,
		                  L"%s Chatted in Room#%d: %s\n",
		                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()),
		                  room->GetID(),
		                  UTF8_TO_WCHAR(packet.chat().c_str()));

		ProjectJ::S_ROOM_CHAT sendPacket;
		sendPacket.set_account_id(gameSession->GetID());
		sendPacket.set_nickname(gameSession->GetNickname());
		sendPacket.set_room_id(room->GetID());
		sendPacket.set_chat(packet.chat());

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		room->DoTaskAsync(&Room::BroadcastHere, sendBuffer);
	}

	return true;
}

/* MATCH PROTOCOLS */

bool Handle_C_MATCH_READY_TO_RECEIVE(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_READY_TO_RECEIVE& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		match->DoTaskAsync(&Match::PlayerReadyToRecieve, gameSession);
	}

	return true;
}

bool Handle_C_MATCH_READY_TO_START(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_READY_TO_START& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		match->DoTaskAsync(&Match::PlayerReadyToStart, gameSession);
	}

	return true;
}

bool Handle_C_MATCH_INFO(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_INFO& packet)
{
	return true;
}

bool Handle_C_MATCH_ITEM_PICKUP(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_PICKUP& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		match->PlayerPickupItem(packet.player_index(), packet.item_index());
	}

	return true;
}

bool Handle_C_MATCH_ITEM_MOVE(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_MOVE& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		match->PlayerMoveItem(packet.player_index(),
		                      packet.from_index(),
		                      packet.to_index(),
		                      packet.item_index(),
		                      packet.target_top_left_index(),
		                      packet.is_item_rotated());
	}
	return true;
}

bool Handle_C_MATCH_ITEM_DROP(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_DROP& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
	}
	return true;
}
