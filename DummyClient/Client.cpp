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

	if (res->status == 201)
	{
		cout << param + " 회원가입 성공" << endl;
	}
	else
	{
		cout << param + " 회원가입 실패" << endl;
	}
}

void ClientSession::TestLoginHttp(string prefix, int index)
{
	string param = prefix + to_string(index);

	httplib::Client cli("http://127.0.0.1:8000");
	string raw = "{\"name\": \"" + param + "\", \"password\" : \"" + param + "\"}";

	auto res = cli.Post("/api/users/login", raw, "application/json");

	if (res->status == 200)
	{
		cout << param + " 로그인 성공" << endl;
		nlohmann::json j = nlohmann::json::parse(res->body);

		name = j["name"];
		nickname = j["nickname"];
		token = j["token"];
		id = j["player_id"];
	}
	else
	{
		cout << param + " 로그인 실패" << endl;
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
	cout << "TEST CREATE ROOM" << endl;
	ProjectJ::C_LOBBY_CREATE_ROOM packet;
	packet.set_account_id(id);
	packet.set_title(title);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestLeaveRoom()
{
	cout << "TEST LEAVE ROOM" << endl;
	ProjectJ::C_ROOM_LEAVE packet;
	packet.set_account_id(id);
	packet.set_room_id(roomID);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestEnterRoom(int roomID)
{
	cout << "TEST ENTER ROOM" << endl;
	cout << roomID << "번 방 입장 시도" << endl;
	ProjectJ::C_LOBBY_ENTER_ROOM packet;
	packet.set_account_id(id);
	packet.set_room_id(roomID);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestRefreshRoomList()
{
	cout << "TEST REFRESH ROOM LIST" << endl;
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
