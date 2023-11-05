#include "pch.h"
#include "ServerPacketHandler.h"
#include "Client.h"
#include "Menu.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(shared_ptr<SessionBase>& session, BYTE* bufer, int numOfBytes)
{
	return true;
}

bool Handle_S_VERIFY_TOKEN(shared_ptr<SessionBase>& session, ProjectJ::S_VERIFY_TOKEN& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	if (packet.result())
	{
		cout << clientSession->nickname << " 토큰 인증 성공" << endl;
	}

	return true;
}

bool Handle_S_LOBBY_CHAT(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_CHAT& packet)
{
	return true;
}

bool Handle_S_LOBBY_REFRESH_ROOM(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_REFRESH_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	for (auto room : packet.rooms())
	{
	}

	return true;
}

bool Handle_S_LOBBY_CREATE_ROOM(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_CREATE_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	if (packet.result())
	{
		clientSession->PrintNickname();
		cout << "방 생성 성공/ 방 번호: " << packet.info().room_id() << endl;
		clientSession->roomID = packet.info().room_id();
	}
	return true;
}

bool Handle_S_LOBBY_ENTER_ROOM(shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_ENTER_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	if (packet.result())
	{
		clientSession->PrintNickname();
		cout << packet.info().room_id() << "번 방 입장 성공 " << endl;
		clientSession->roomID = packet.info().room_id();
	}
	return true;
}

bool Handle_S_ROOM_LEAVE(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_LEAVE& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	if (packet.result())
	{
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

bool Handle_S_ROOM_STANDBY_MATCH(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_STANDBY_MATCH& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	clientSession->PrintNickname();
	cout << clientSession->roomID << "번 방 스탠바이 " << packet.count() << endl;
	return true;
}

bool Handle_S_ROOM_START_MATCH(shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_START_MATCH& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	clientSession->PrintNickname();
	cout << clientSession->roomID << "번 방 게임 시작 " << endl;
	return true;
}

bool Handle_S_MATCH_INIT_PLAYER_INDEX(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_INIT_PLAYER_INDEX& packet)
{
	return true;
}

bool Handle_S_MATCH_INIT_ITEMS(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_INIT_ITEMS& packet)
{
	return true;
}

bool Handle_S_MATCH_ALL_LOADING_COMPLETE(shared_ptr<SessionBase>& session,
                                         ProjectJ::S_MATCH_ALL_LOADING_COMPLETE& packet)
{
	return true;
}

bool Handle_S_MATCH_START(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_START& packet)
{
	return true;
}

bool Handle_S_MATCH_INFO(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_INFO& packet)
{
	return true;
}

bool Handle_S_MATCH_END(shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_END& packet)
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
