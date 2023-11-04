#include "pch.h"
#include "Client.h"

void ClientSession::TestCreateRoom()
{
	cout << "TEST CREATE ROOM" << endl;
	ProjectJ::C_LOBBY_CREATE_ROOM packet;
	packet.set_account_id(accountID);
	packet.set_title("test room");
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestLeaveRoom()
{
	cout << "TEST LEAVE ROOM" << endl;
	ProjectJ::C_ROOM_LEAVE packet;
	packet.set_account_id(accountID);
	packet.set_room_id(roomID);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestEnterRoom(int roomID)
{
	cout << "TEST ENTER ROOM" << endl;
	cout << roomID << "번 방 입장 시도" << endl;
	ProjectJ::C_LOBBY_ENTER_ROOM packet;
	packet.set_account_id(accountID);
	packet.set_room_id(roomID);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Send(sendBuffer);
}

void ClientSession::TestRefreshRoomList()
{
	cout << "TEST REFRESH ROOM LIST" << endl;
	ProjectJ::C_LOBBY_REFRESH_ROOM packet;
	packet.set_account_id(accountID);
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
