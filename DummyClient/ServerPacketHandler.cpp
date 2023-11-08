#include "pch.h"
#include "ServerPacketHandler.h"
#include "Client.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(const shared_ptr<SessionBase>& session, BYTE* bufer, int numOfBytes)
{
	return true;
}

bool Handle_S_VERIFY_TOKEN(const shared_ptr<SessionBase>& session, ProjectJ::S_VERIFY_TOKEN& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	if (packet.result())
	{
		cout << clientSession->nickname << " 토큰 인증 성공" << endl;
		clientSession->isVerified = true;
	}

	return true;
}

bool Handle_S_LOBBY_CHAT(const shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_CHAT& packet)
{
	return true;
}

bool Handle_S_LOBBY_REFRESH_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_REFRESH_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	for (auto room : packet.rooms())
	{
	}

	return true;
}

bool Handle_S_LOBBY_CREATE_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_CREATE_ROOM& packet)
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

bool Handle_S_LOBBY_ENTER_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_ENTER_ROOM& packet)
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

bool Handle_S_ROOM_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_LEAVE& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	if (packet.result())
	{
	}
	return true;
}

bool Handle_S_ROOM_OTHER_ENTER(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_OTHER_ENTER& packet)
{
	return true;
}

bool Handle_S_ROOM_OTHER_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_OTHER_LEAVE& packet)
{
	return true;
}

bool Handle_S_ROOM_READY(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_READY& packet)
{
	return true;
}

bool Handle_S_ROOM_CHAT(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_CHAT& packet)
{
	return true;
}

bool Handle_S_ROOM_STANDBY_MATCH(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_STANDBY_MATCH& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	clientSession->PrintNickname();
	cout << clientSession->roomID << "번 방 스탠바이 " << packet.count() << endl;
	return true;
}

bool Handle_S_ROOM_START_MATCH(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_START_MATCH& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	clientSession->PrintNickname();
	cout << clientSession->roomID << "번 방 게임 시작 " << endl;
	return true;
}

bool Handle_S_MATCH_INIT_PLAYER_INDEX(const shared_ptr<SessionBase>& session,
                                      ProjectJ::S_MATCH_INIT_PLAYER_INDEX& packet)
{
	return true;
}

bool Handle_S_MATCH_INIT_ITEMS(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_INIT_ITEMS& packet)
{
	return true;
}

bool Handle_S_MATCH_ALL_LOADING_COMPLETE(const shared_ptr<SessionBase>& session,
                                         ProjectJ::S_MATCH_ALL_LOADING_COMPLETE& packet)
{
	return true;
}

bool Handle_S_MATCH_START(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_START& packet)
{
	return true;
}

bool Handle_S_MATCH_INFO(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_INFO& packet)
{
	return true;
}

bool Handle_S_MATCH_END(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_END& packet)
{
	return true;
}

bool Handle_S_MATCH_ITEM_PICKUP(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_PICKUP& packet)
{
	return true;
}

bool Handle_S_MATCH_ITEM_MOVE(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_MOVE& packet)
{
	return true;
}

bool Handle_S_MATCH_ITEM_DROP(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_DROP& packet)
{
	return true;
}
