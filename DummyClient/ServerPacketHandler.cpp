#include "pch.h"
#include "ServerPacketHandler.h"

#include <random>

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
		cout << "방 생성 성공/ 방 번호: " << packet.room_id() << endl;
		clientSession->roomID = packet.room_id();
		clientSession->roomSlotIndex = packet.slot_index();

		clientSession->TestRoomReadyToReceive();
	}
	return true;
}

bool Handle_S_LOBBY_ENTER_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::S_LOBBY_ENTER_ROOM& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	if (packet.result())
	{
		clientSession->PrintNickname();
		cout << packet.room_id() << "번 방 입장 성공 " << endl;
		clientSession->roomID = packet.room_id();
		clientSession->roomSlotIndex = packet.slot_index();

		clientSession->TestRoomReadyToReceive();
	}
	else
	{
		clientSession->PrintNickname();
		cout << packet.room_id() << "번 방 입장 실패 " << endl;
	}
	return true;
}

bool Handle_S_ROOM_INFO(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_INFO& packet)
{
	cout << "추격자: " << packet.info().chaser().player().nickname() << endl;
	cout << "도망자1: " << packet.info().fugitive_first().player().nickname() << endl;
	cout << "도망자2: " << packet.info().fugitive_second().player().nickname() << endl;
	cout << "도망자3: " << packet.info().fugitive_third().player().nickname() << endl;

	return true;
}

bool Handle_S_ROOM_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_LEAVE& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	if (packet.result())
	{
		cout << "대기방 퇴장 성공" << endl;
	}
	return true;
}

bool Handle_S_ROOM_OTHER_ENTER(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_OTHER_ENTER& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	cout << clientSession->roomID << "번 방: 슬롯 " << packet.slot_index() << "에 " << packet.other().nickname() << "입장" << endl;

	return true;
}

bool Handle_S_ROOM_OTHER_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_OTHER_LEAVE& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	cout << clientSession->roomID << "번 방: 슬롯 " << packet.slot_index() << " 퇴장" << endl;
	return true;
}

bool Handle_S_ROOM_READY(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_READY& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	cout << clientSession->roomID << "번 방: 슬롯 " << packet.slot_index() << " 레디: " << packet.is_ready() << endl;
	return true;
}

bool Handle_S_ROOM_CHAT(const shared_ptr<SessionBase>& session, ProjectJ::S_ROOM_CHAT& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	{
		cout << packet.room_id() << "번 방의 채팅 " << packet.nickname() << ": " << packet.chat() << endl;
	}
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

	if (packet.start())
	{
		ProjectJ::C_MATCH_READY_TO_RECEIVE sendPacket;

		sendPacket.set_account_id(clientSession->id);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(sendPacket);
		session->Send(sendBuffer);
	}

	return true;
}

bool Handle_S_MATCH_ALL_READY_TO_RECEIVE(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ALL_READY_TO_RECEIVE& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	clientSession->matchIndex = packet.your_player_index();
	{
		int index = packet.your_player_index();

		ProjectJ::C_MATCH_CHARACTER_SPAWN_POSITION sendPacket;
		auto position = new ProjectJ::Vector();
		auto rotation = new ProjectJ::Rotator();

		if (index == 0)
		{
			auto character = packet.info().chaser();
			auto vector = character.position();
			auto rotator = character.rotation();
			position->set_x(vector.x());
			position->set_y(vector.y());
			position->set_z(100.0f);

			rotation->set_pitch(rotator.pitch());
			rotation->set_roll(rotator.roll());
			rotation->set_yaw(rotator.yaw());
		}
		else if (index == 1)
		{
			auto character = packet.info().fugitive_first();
			auto vector = character.position();
			auto rotator = character.rotation();
			position->set_x(vector.x());
			position->set_y(vector.y());
			position->set_z(100.0f);

			rotation->set_pitch(rotator.pitch());
			rotation->set_roll(rotator.roll());
			rotation->set_yaw(rotator.yaw());
		}
		else if (index == 2)
		{
			auto character = packet.info().fugitive_second();
			auto vector = character.position();
			auto rotator = character.rotation();
			position->set_x(vector.x());
			position->set_y(vector.y());
			position->set_z(100.0f);

			rotation->set_pitch(rotator.pitch());
			rotation->set_roll(rotator.roll());
			rotation->set_yaw(rotator.yaw());
		}
		else
		{
			auto character = packet.info().fugitive_third();
			auto vector = character.position();
			auto rotator = character.rotation();
			position->set_x(vector.x());
			position->set_y(vector.y());
			position->set_z(100.0f);

			rotation->set_pitch(rotator.pitch());
			rotation->set_roll(rotator.roll());
			rotation->set_yaw(rotator.yaw());
		}

		sendPacket.set_allocated_rotation(rotation);
		sendPacket.set_allocated_position(position);
		sendPacket.set_player_index(index);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(sendPacket);


		clientSession->Send(sendBuffer);
		cout << "서버로 위치 보냄" << endl;
	}

	{
		ProjectJ::C_MATCH_READY_TO_START sendPacket;
		sendPacket.set_player_index(clientSession->matchIndex);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(sendPacket);
		session->Send(sendBuffer);
	}


	return true;
}

bool Handle_S_MATCH_ITEM_GENERATED(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_GENERATED& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	return true;
}

bool Handle_S_MATCH_START(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_START& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	/*this_thread::sleep_for(3s);
	for (int i = 0; i < 30; i++)
	{
		ProjectJ::C_MATCH_ITEM_PICKUP sendPacket;

		sendPacket.set_item_index(i);
		sendPacket.set_player_index(clientSession->matchIndex);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(sendPacket);
		session->Send(sendBuffer);
	}*/

	return true;
}

bool Handle_S_MATCH_INFO(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_INFO& packet)
{
	return true;
}

bool Handle_S_MATCH_END(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_END& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	this_thread::sleep_for(1s);
	clientSession->TestRoomReadyToReceive();
	clientSession->TestRoomReady();

	return true;
}

bool Handle_S_MATCH_ITEM_SOMEONE_PICKUP(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP& packet)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);
	if (clientSession->matchIndex == packet.player_index())
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		uniform_int_distribution<int> slotDis(0, 100);
		uniform_int_distribution<int> toDis(4, 7);
		int slotindex = slotDis(gen);
		int toIndex = toDis(gen);
		ProjectJ::C_MATCH_ITEM_MOVE sendPacket;
		sendPacket.set_player_index(clientSession->matchIndex);
		sendPacket.set_from_index(clientSession->matchIndex);
		sendPacket.set_to_index(5);
		sendPacket.set_target_top_left_index(slotindex);
		sendPacket.set_is_item_rotated(false);
		sendPacket.set_item_index(packet.item_index());

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(sendPacket);
		session->Send(sendBuffer);

		cout << "item 이동 시도" << clientSession->matchIndex << "에서 " << toIndex << "의 " << slotindex << "칸으로" << endl;
	}

	return true;
}

bool Handle_S_MATCH_ITEM_SOMEONE_MOVE(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE& packet)
{
	return true;
}

bool Handle_S_MATCH_ITEM_SOMEONE_DROP(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_ITEM_SOMEONE_DROP& packet)
{
	return true;
}

bool Handle_S_MATCH_SCALE_ON_CHANGED(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_SCALE_ON_CHANGED& packet)
{
	return true;
}

bool Handle_S_MATCH_CHASER_ATTACK(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_CHASER_ATTACK& packet)
{
	return true;
}

bool Handle_S_MATCH_CHASER_HIT(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_CHASER_HIT& packet)
{
	return true;
}

bool Handle_S_MATCH_FUGITIVE_ESCAPE(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_FUGITIVE_ESCAPE& packet)
{
	return true;
}

bool Handle_S_MATCH_CHASER_INSTALL_CCTV(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_CHASER_INSTALL_CCTV& packet)
{
	return true;
}

bool Handle_S_MATCH_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::S_MATCH_LEAVE& packet)
{
	return true;
}
