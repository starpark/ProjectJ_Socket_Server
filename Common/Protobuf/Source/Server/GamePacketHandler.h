﻿#pragma once
#include "Message.pb.h"

using PacketHandlerFunc = function<bool(shared_ptr<SessionBase>&, BYTE*, int)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16_t
{
	PKT_C_VERIFY_TOKEN = 1000,
	PKT_S_VERIFY_TOKEN = 1001,
	PKT_C_LOBBY_CHAT = 1002,
	PKT_S_LOBBY_CHAT = 1003,
	PKT_C_LOBBY_REFRESH_ROOM = 1004,
	PKT_S_LOBBY_REFRESH_ROOM = 1005,
	PKT_C_LOBBY_CREATE_ROOM = 1006,
	PKT_S_LOBBY_CREATE_ROOM = 1007,
	PKT_C_LOBBY_ENTER_ROOM = 1008,
	PKT_S_LOBBY_ENTER_ROOM = 1009,
	PKT_C_ROOM_LEAVE = 1010,
	PKT_S_ROOM_LEAVE = 1011,
	PKT_S_ROOM_OTHER_ENTER = 1012,
	PKT_S_ROOM_OTHER_LEAVE = 1013,
	PKT_C_ROOM_READY = 1014,
	PKT_S_ROOM_READY = 1015,
	PKT_C_ROOM_CHAT = 1016,
	PKT_S_ROOM_CHAT = 1017,
	PKT_S_ROOM_STANDBY_MATCH = 1018,
	PKT_S_ROOM_START_MATCH = 1019,
	PKT_S_MATCH_INIT_PLAYER_INDEX = 1020,
	PKT_S_MATCH_INIT_ITEMS = 1021,
	PKT_C_MATCH_LOADING_COMPLETE = 1022,
	PKT_S_MATCH_ALL_LOADING_COMPLETE = 1023,
	PKT_S_MATCH_START = 1024,
	PKT_S_MATCH_INFO = 1025,
	PKT_S_MATCH_END = 1026,
	PKT_C_MATCH_ITEM_PICKUP = 1027,
	PKT_S_MATCH_ITEM_PICKUP = 1028,
	PKT_C_MATCH_ITEM_MOVE = 1029,
	PKT_S_MATCH_ITEM_MOVE = 1030,
	PKT_C_MATCH_ITEM_DROP = 1031,
	PKT_S_MATCH_ITEM_DROP = 1032,
};

bool Handle_INVALID(const shared_ptr<SessionBase>& session, BYTE* bufer, int numOfBytes);
bool Handle_C_VERIFY_TOKEN(const shared_ptr<SessionBase>& session, ProjectJ::C_VERIFY_TOKEN& packet);
bool Handle_C_LOBBY_CHAT(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_CHAT& packet);
bool Handle_C_LOBBY_REFRESH_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_REFRESH_ROOM& packet);
bool Handle_C_LOBBY_CREATE_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_CREATE_ROOM& packet);
bool Handle_C_LOBBY_ENTER_ROOM(const shared_ptr<SessionBase>& session, ProjectJ::C_LOBBY_ENTER_ROOM& packet);
bool Handle_C_ROOM_LEAVE(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_LEAVE& packet);
bool Handle_C_ROOM_READY(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_READY& packet);
bool Handle_C_ROOM_CHAT(const shared_ptr<SessionBase>& session, ProjectJ::C_ROOM_CHAT& packet);
bool Handle_C_MATCH_LOADING_COMPLETE(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_LOADING_COMPLETE& packet);
bool Handle_C_MATCH_ITEM_PICKUP(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_PICKUP& packet);
bool Handle_C_MATCH_ITEM_MOVE(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_MOVE& packet);
bool Handle_C_MATCH_ITEM_DROP(const shared_ptr<SessionBase>& session, ProjectJ::C_MATCH_ITEM_DROP& packet);


// 소켓 수신 데이터 처리 및 송신 버퍼 생성 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: 2023-11-08 자동 생성
class GamePacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[PKT_C_VERIFY_TOKEN] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_VERIFY_TOKEN>(Handle_C_VERIFY_TOKEN, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_LOBBY_CHAT] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_LOBBY_CHAT>(Handle_C_LOBBY_CHAT, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_LOBBY_REFRESH_ROOM] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_LOBBY_REFRESH_ROOM>(Handle_C_LOBBY_REFRESH_ROOM, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_LOBBY_CREATE_ROOM] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_LOBBY_CREATE_ROOM>(Handle_C_LOBBY_CREATE_ROOM, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_LOBBY_ENTER_ROOM] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_LOBBY_ENTER_ROOM>(Handle_C_LOBBY_ENTER_ROOM, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_ROOM_LEAVE] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_ROOM_LEAVE>(Handle_C_ROOM_LEAVE, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_ROOM_READY] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_ROOM_READY>(Handle_C_ROOM_READY, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_ROOM_CHAT] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_ROOM_CHAT>(Handle_C_ROOM_CHAT, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_MATCH_LOADING_COMPLETE] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_MATCH_LOADING_COMPLETE>(Handle_C_MATCH_LOADING_COMPLETE, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_MATCH_ITEM_PICKUP] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_MATCH_ITEM_PICKUP>(Handle_C_MATCH_ITEM_PICKUP, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_MATCH_ITEM_MOVE] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_MATCH_ITEM_MOVE>(Handle_C_MATCH_ITEM_MOVE, session, buffer, numOfBytes);};
		GPacketHandler[PKT_C_MATCH_ITEM_DROP] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::C_MATCH_ITEM_DROP>(Handle_C_MATCH_ITEM_DROP, session, buffer, numOfBytes);};
	}

	static bool HandlePacket(shared_ptr<SessionBase>& session, BYTE* buffer, int numOfBytes)
	{
		auto header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->type](session, buffer, numOfBytes);
	}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_VERIFY_TOKEN& packet) {return MakeSendBuffer(packet, PKT_S_VERIFY_TOKEN);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_LOBBY_CHAT& packet) {return MakeSendBuffer(packet, PKT_S_LOBBY_CHAT);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_LOBBY_REFRESH_ROOM& packet) {return MakeSendBuffer(packet, PKT_S_LOBBY_REFRESH_ROOM);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_LOBBY_CREATE_ROOM& packet) {return MakeSendBuffer(packet, PKT_S_LOBBY_CREATE_ROOM);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_LOBBY_ENTER_ROOM& packet) {return MakeSendBuffer(packet, PKT_S_LOBBY_ENTER_ROOM);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_ROOM_LEAVE& packet) {return MakeSendBuffer(packet, PKT_S_ROOM_LEAVE);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_ROOM_OTHER_ENTER& packet) {return MakeSendBuffer(packet, PKT_S_ROOM_OTHER_ENTER);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_ROOM_OTHER_LEAVE& packet) {return MakeSendBuffer(packet, PKT_S_ROOM_OTHER_LEAVE);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_ROOM_READY& packet) {return MakeSendBuffer(packet, PKT_S_ROOM_READY);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_ROOM_CHAT& packet) {return MakeSendBuffer(packet, PKT_S_ROOM_CHAT);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_ROOM_STANDBY_MATCH& packet) {return MakeSendBuffer(packet, PKT_S_ROOM_STANDBY_MATCH);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_ROOM_START_MATCH& packet) {return MakeSendBuffer(packet, PKT_S_ROOM_START_MATCH);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_INIT_PLAYER_INDEX& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_INIT_PLAYER_INDEX);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_INIT_ITEMS& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_INIT_ITEMS);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_ALL_LOADING_COMPLETE& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_ALL_LOADING_COMPLETE);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_START& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_START);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_INFO& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_INFO);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_END& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_END);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_ITEM_PICKUP& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_ITEM_PICKUP);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_ITEM_MOVE& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_ITEM_MOVE);}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_MATCH_ITEM_DROP& packet) {return MakeSendBuffer(packet, PKT_S_MATCH_ITEM_DROP);}

private:
	template <typename PacketMessage, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, shared_ptr<SessionBase>& session, BYTE* buffer, int numOfBytes)
	{
		PacketMessage packet;
		if (packet.ParseFromArray(buffer + sizeof(PacketHeader), numOfBytes - sizeof(PacketHeader)) == false)
		{
			return false;
		}

		return func(session, packet);
	}

	template <typename PacketMessage>
	static shared_ptr<SendBuffer> MakeSendBuffer(PacketMessage& packet, uint16_t type)
	{
		const uint16_t dataSize = static_cast<uint16_t>(packet.ByteSizeLong());
		const uint16_t packetSize = dataSize + sizeof(PacketHeader);

		auto sendBuffer = GSendBufferManager->GetSendBuffer(packetSize);
		auto header = reinterpret_cast<PacketHeader*>(sendBuffer->GetBuffer());
		header->size = packetSize;
		header->type = type;

		ASSERT_CRASH(packet.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};