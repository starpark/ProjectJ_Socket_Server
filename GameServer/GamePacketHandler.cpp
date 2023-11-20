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
	GLogHelper->Print(LogCategory::LOG_INFO,
	                  L"Authentication attempts: %s:%hu\n",
	                  UTF8_TO_WCHAR(gameSession->GetNetAddress().GetIpAddress().c_str()), gameSession->GetNetAddress().GetPort());
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

			GLogHelper->Print(LogCategory::LOG_SUCCESS, L"Authentication Success: %s:%hu AccountID: %d Nickname: %s\n",
			                  UTF8_TO_WCHAR(gameSession->GetNetAddress().GetIpAddress().c_str()),
			                  gameSession->GetNetAddress().GetPort(),
			                  gameSession->GetID(),
			                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()));


			verifyPacket.set_result(true);
			gameSession->SetIsVerified(true);
		}
		catch (nlohmann::json::exception& e)
		{
			// parsing type error
			GLogHelper->Print(LogCategory::LOG_WARN, L"Authentication Failure: %s\n", UTF8_TO_WCHAR(e.what()));
			verifyPacket.set_result(false);
		}
		catch (int e)
		{
			GLogHelper->Print(LogCategory::LOG_WARN, L"Authentication Failure: Error Number %d\n", e);
			verifyPacket.set_result(false);
		}
		catch (...)
		{
			GLogHelper->Print(LogCategory::LOG_WARN, L"Authentication Failure: Unexpected Error\n");
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
		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"%s Chatted in Lobby: %s\n",
		                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()),
		                  UTF8_TO_WCHAR(packet.chat().c_str()));

		lobby->DoTaskAsync([lobby, gameSession, packet]()
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
		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"%s#%d Has Refreshed Room List\n",
		                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()), gameSession->GetID());

		lobby->DoTaskCallback(&Lobby::GetRoomList, [gameSession](vector<shared_ptr<Room>> rooms)
		{
			ProjectJ::S_LOBBY_REFRESH_ROOM sendPacket;
			for (auto room : rooms)
			{
				if (room->GetState() == RoomState::INVALID)
				{
					continue;
				}
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
				newRoom->DoTaskCallback(&Room::Enter, [newRoom, gameSession](int slotIndex)
				{
					if (slotIndex != INVALID_SLOT_INDEX && slotIndex < MAX_PLAYER_NUMBER)
					{
						ProjectJ::S_LOBBY_CREATE_ROOM sendPacket;

						sendPacket.set_result(true);
						sendPacket.set_room_id(newRoom->GetID());
						sendPacket.set_slot_index(slotIndex);

						auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
						gameSession->Send(sendBuffer);
					}
				}, gameSession);
			}
			else
			{
				ProjectJ::S_LOBBY_CREATE_ROOM sendPacket;
				sendPacket.set_result(false);
				sendPacket.set_slot_index(INVALID_SLOT_INDEX);

				auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
				gameSession->Send(sendBuffer);
			}
		}, gameSession, packet.title());
	}
	else
	{
		ProjectJ::S_LOBBY_CREATE_ROOM sendPacket;
		sendPacket.set_result(false);
		sendPacket.set_slot_index(INVALID_SLOT_INDEX);

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
				room->DoTaskCallback(&Room::Enter, [room, gameSession](int slotIndex)
				{
					ProjectJ::S_LOBBY_ENTER_ROOM sendPacket;

					if (slotIndex != INVALID_SLOT_INDEX && slotIndex < MAX_PLAYER_NUMBER)
					{
						sendPacket.set_result(true);
						sendPacket.set_room_id(room->GetID());
						sendPacket.set_slot_index(slotIndex);
					}
					else
					{
						sendPacket.set_result(false);
						sendPacket.set_room_id(INVALID_ROOM_ID);
						sendPacket.set_slot_index(INVALID_SLOT_INDEX);
					}

					auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
					gameSession->Send(sendBuffer);
				}, gameSession);
			}
			else
			{
				GLogHelper->Print(LogCategory::LOG_INFO,
				                  L"%s Accessed An Invalid Room.\n",
				                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()));

				ProjectJ::S_LOBBY_ENTER_ROOM sendPacket;
				sendPacket.set_result(false);
				sendPacket.set_room_id(INVALID_ROOM_ID);
				sendPacket.set_slot_index(INVALID_SLOT_INDEX);

				auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
				gameSession->Send(sendBuffer);
			}
		}, packet.room_id());
	}
	else
	{
		if (auto room = gameSession->TryGetRoom())
		{
			GLogHelper->Print(LogCategory::LOG_INFO,
			                  L"%s Already In Room#%d, But Requested Enter To Room#%d.\n",
			                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()), room->GetID(), packet.room_id());

			return false;
		}

		gameSession->Disconnect();
		return false;
	}

	return true;
}

bool Handle_C_ROOM_READY_TO_RECEIVE(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_READY_TO_RECEIVE& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Room> room = gameSession->TryGetRoom();

	if (!gameSession && gameSession->IsVerified() == false || gameSession->GetID() != packet.account_id())
	{
		gameSession->Disconnect();
		return false;
	}

	if (room)
	{
		room->DoTaskAsync(&Room::SessionReadyToReceive, gameSession);
	}
	else
	{
		session->Disconnect();
		return false;
	}

	return true;
}

bool Handle_C_ROOM_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_LEAVE& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	shared_ptr<Room> room = gameSession->TryGetRoom();

	if (!gameSession && gameSession->IsVerified() == false || gameSession->GetID() != packet.account_id())
	{
		gameSession->Disconnect();
		return false;
	}

	if (room)
	{
		if (room->GetID() != packet.room_id())
		{
			GLogHelper->Print(LogCategory::LOG_INFO,
			                  L"%s Try To Leaved An Invalid Room. Target: #%d Received: #%d\n",
			                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()), room->GetID(), packet.room_id());

			session->Disconnect();
			return false;
		}

		room->DoTaskCallback(&Room::Leave, [gameSession](int index)
		{
			ProjectJ::S_ROOM_LEAVE sendPacket;
			sendPacket.set_result(index > INVALID_SLOT_INDEX ? true : false);

			auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
			gameSession->Send(sendBuffer);
		}, gameSession, packet.slot_index());
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
		room->DoTaskAsync(&Room::ToggleReady, gameSession, packet.slot_index());
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
		GLogHelper->Print(LogCategory::LOG_INFO,
		                  L"%s Chatted In Room#%d: %s\n",
		                  UTF8_TO_WCHAR(gameSession->GetNickname().c_str()),
		                  room->GetID(),
		                  UTF8_TO_WCHAR(packet.chat().c_str()));

		ProjectJ::S_ROOM_CHAT sendPacket;
		sendPacket.set_account_id(gameSession->GetID());
		sendPacket.set_nickname(gameSession->GetNickname());
		sendPacket.set_room_id(room->GetID());
		sendPacket.set_chat(packet.chat());

		auto sendBuffer = GamePacketHandler::MakeSendBuffer(sendPacket);
		room->DoTaskAsync(&Room::Broadcast, sendBuffer);
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
		match->DoTaskAsync(&Match::PlayerReadyToReceive, gameSession);
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
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		Vector position(packet.info().position().x(), packet.info().position().y(), packet.info().position().z());
		Rotator rotation(packet.info().rotation().roll(), packet.info().rotation().pitch(), packet.info().rotation().yaw());

		match->PlayerSetTransform(gameSession, packet.player_index(), move(position), move(rotation));
	}

	return true;
}

bool Handle_C_MATCH_ITEM_PICKUP(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_PICKUP& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		match->PlayerPickUpItem(gameSession, packet.player_index(), packet.item_index());
	}

	return true;
}

bool Handle_C_MATCH_ITEM_MOVE(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_MOVE& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		match->PlayerMoveItem(gameSession,
		                      packet.player_index(),
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
		Vector position(packet.drop_item_position().x(), packet.drop_item_position().y(), packet.drop_item_position().z());
		Rotator rotation(packet.drop_item_rotation().roll(), packet.drop_item_rotation().pitch(), packet.drop_item_rotation().yaw());

		match->PlayerDropItem(gameSession,
		                      packet.player_index(),
		                      packet.item_index(),
		                      position,
		                      rotation);
	}

	return true;
}

bool Handle_C_MATCH_CHASER_ATTACK(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_CHASER_ATTACK& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		Vector position(packet.attack_position().x(), packet.attack_position().y(), packet.attack_position().z());
		Rotator rotation(packet.attack_rotation().roll(), packet.attack_rotation().pitch(), packet.attack_rotation().yaw());

		match->ChaserAttack(gameSession, position, rotation);
	}

	return true;
}

bool Handle_C_MATCH_CHASER_HIT(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_CHASER_HIT& packet)
{
	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	auto match = gameSession->TryGetMatch();

	if (match)
	{
		Vector position(packet.attack_position().x(), packet.attack_position().y(), packet.attack_position().z());
		Rotator rotation(packet.attack_rotation().roll(), packet.attack_rotation().pitch(), packet.attack_rotation().yaw());

		match->HitValidation(gameSession, position, rotation, packet.hit_player_index());
	}

	return true;
}
