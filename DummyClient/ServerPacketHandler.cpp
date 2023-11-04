#include "pch.h"
#include "ServerPacketHandler.h"
#include "Client.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(shared_ptr<SessionBase>& session, BYTE* bufer, int numOfBytes)
{
	return true;
}

bool Handle_S_VERIFY_TOKEN(shared_ptr<SessionBase>& session, ProjectJ::S_VERIFY_TOKEN& packet)
{
	return true;
}

bool Handle_S_LOBBY_CHAT(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_CHAT& packet)
{
	return true;
}

bool Handle_S_LOBBY_REFRESH_ROOM(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_REFRESH_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	clientSession->roomList.clear();

	for (auto room : packet.rooms())
	{
		cout << room.id() << "번 방 확인됨" << endl;
		clientSession->roomList.push_back(room.id());
	}

	return true;
}

bool Handle_S_LOBBY_CREATE_ROOM(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_CREATE_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	cout << "방 생성: " << packet.result() << endl;
	if (packet.result())
	{
		clientSession->isRoom = true;
		clientSession->roomID = packet.info().room_id();
		cout << "방 정보: " << packet.info().room_id() << ", " << packet.info().title() << endl;
		cout << "1번 유저: " << packet.info().chaser().player().nickname() << endl;
		cout << "2번 유저: " << packet.info().fugitive_first().player().nickname() << endl;
		cout << "3번 유저: " << packet.info().fugitive_second().player().nickname() << endl;
		cout << "4번 유저: " << packet.info().fugitive_third().player().nickname() << endl;
	}
	return true;
}

bool Handle_S_LOBBY_ENTER_ROOM(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_ENTER_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	cout << "방 입장: " << packet.result() << endl;
	if (packet.result())
	{
		clientSession->isRoom = true;
		clientSession->roomID = packet.info().room_id();
		cout << "방 정보: " << packet.info().room_id() << ", " << packet.info().title() << endl;
		cout << "1번 유저: " << packet.info().chaser().player().nickname() << endl;
		cout << "2번 유저: " << packet.info().fugitive_first().player().nickname() << endl;
		cout << "3번 유저: " << packet.info().fugitive_second().player().nickname() << endl;
		cout << "4번 유저: " << packet.info().fugitive_third().player().nickname() << endl;
	}
	return true;
}

bool Handle_S_ROOM_LEAVE(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_LEAVE& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	cout << "방 퇴장: " << packet.result() << endl;
	if (packet.result())
	{
		clientSession->isRoom = false;
	}
	return true;
}

bool Handle_S_ROOM_OTHER_ENTER(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_OTHER_ENTER& packet)
{
	return true;
}

bool Handle_S_ROOM_OTHER_LEAVE(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_OTHER_LEAVE& packet)
{
	return true;
}

bool Handle_S_ROOM_READY(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_READY& packet)
{
	return true;
}

bool Handle_S_ROOM_CHAT(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_CHAT& packet)
{
	return true;
}

bool Handle_S_MATCH_INIT_GENERATED_ITEMS(shared_ptr<SessionBase>& session,
                                         ProjectJ::S_MATCH_INIT_GENERATED_ITEMS& packet)
{
	return true;
}

bool Handle_S_MATCH_ITEM_PICKUP(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_PICKUP& packet)
{
	return true;
}

bool Handle_S_MATCH_ITEM_MOVE(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_MOVE& packet)
{
	return true;
}

bool Handle_S_MATCH_ITEM_DROP(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_DROP& packet)
{
	return true;
}
