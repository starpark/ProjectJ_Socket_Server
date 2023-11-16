#include "pch.h"
#include "Client.h"
#include <nlohmann/json.hpp>
#include "httplib.h"

void ClientSession::TestCreateAccount(string prefix, int index)
{
	string param = prefix + to_string(index);

	httplib::Client cli("http://127.0.0.1:8000");
	string raw = "{\"name\": \"" + param + "\", \"password\" : \"" + param + "\", \"nickname\": \"" + param +
		"\"}";

	auto res = cli.Post("/api/users/create", raw, "application/json");
}

void ClientSession::TestLoginHttp(string prefix, int index)
{
	string param = prefix + to_string(index);

	httplib::Client cli("http://127.0.0.1:8000");
	string raw = "{\"name\": \"" + param + "\", \"password\" : \"" + param + "\"}";

	auto res = cli.Post("/api/users/login", raw, "application/json");

	if (res->status == 200)
	{
		nlohmann::json j = nlohmann::json::parse(res->body);

		name = j["name"];
		nickname = j["nickname"];
		token = j["token"];
		id = j["player_id"];
	}
}

void ClientSession::TestVerifyToken()
{
	ProjectJ::C_VERIFY_TOKEN packet;
	packet.set_account_id(id);
	packet.set_token(token);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestCreateRoom(string title)
{
	ProjectJ::C_LOBBY_CREATE_ROOM packet;
	packet.set_account_id(id);
	packet.set_title(title);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestLeaveRoom()
{
	ProjectJ::C_ROOM_LEAVE packet;
	packet.set_account_id(id);
	packet.set_room_id(roomID);
	packet.set_slot_index(roomSlotIndex);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestEnterRoom(int roomID)
{
	cout << roomID << "번 방 입장 시도" << endl;
	ProjectJ::C_LOBBY_ENTER_ROOM packet;
	packet.set_account_id(id);
	packet.set_room_id(roomID);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestRoomReadyToReceive()
{
	ProjectJ::C_ROOM_READY_TO_RECEIVE sendPacket;
	sendPacket.set_account_id(id);
	sendPacket.set_room_id(roomID);
	sendPacket.set_slot_index(roomSlotIndex);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(sendPacket);
	Send(sendBuffer);
}

void ClientSession::TestLobbyChat(string msg)
{
	ProjectJ::C_LOBBY_CHAT packet;
	packet.set_account_id(id);
	packet.set_nickname(nickname);
	packet.set_chat(msg);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestRefreshRoomList()
{
	ProjectJ::C_LOBBY_REFRESH_ROOM packet;
	packet.set_account_id(id);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestRoomReady()
{
	ProjectJ::C_ROOM_READY packet;
	packet.set_account_id(id);
	packet.set_room_id(roomID);
	packet.set_slot_index(roomSlotIndex);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestRoomChat(string msg)
{
	ProjectJ::C_ROOM_CHAT packet;
	packet.set_account_id(id);
	packet.set_room_id(roomID);
	packet.set_nickname(nickname);
	packet.set_chat(msg.c_str());
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}


vector<shared_ptr<ClientSession>> ClientService::GetSessions()
{
	READ_LOCK;
	vector<shared_ptr<ClientSession>> sessions;

	for (auto session : sessions_)
	{
		sessions.push_back(static_pointer_cast<ClientSession>(session));
	}

	return sessions;
}
