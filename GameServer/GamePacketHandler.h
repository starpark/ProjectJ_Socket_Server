#pragma once
#include "Message.pb.h"

class GameSession;
using PacketHandlerFunc = function<bool(shared_ptr<class GameSession>&, BYTE*, int)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16_t
{
	PKT_C_VERIFY_TOKEN = 1000,
	PKT_S_VERIFY_TOKEN = 1001,
	PKT_C_LOBBY_CHAT = 1002,
	PKT_S_LOBBY_CHAT = 1003,
};

bool Handle_INVALID(shared_ptr<GameSession>& session, BYTE* bufer, int numOfBytes);
bool Handle_C_VERIFY_TOKEN(shared_ptr<GameSession>& session, ProjectJ::C_VERIFY_TOKEN& packet);
bool Handle_C_LOBBY_CHAT(shared_ptr<GameSession>& session, ProjectJ::C_LOBBY_CHAT& packet);


// 소켓 수신 데이터 처리 및 송신 버퍼 생성 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: 2023-10-10 자동 생성
class GamePacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[PKT_C_VERIFY_TOKEN] = [](shared_ptr<GameSession> session, BYTE* buffer, int numOfBytes)
		{
			return HandlePacket<ProjectJ::C_VERIFY_TOKEN>(Handle_C_VERIFY_TOKEN, session, buffer, numOfBytes);
		};
		GPacketHandler[PKT_C_LOBBY_CHAT] = [](shared_ptr<GameSession> session, BYTE* buffer, int numOfBytes)
		{
			return HandlePacket<ProjectJ::C_LOBBY_CHAT>(Handle_C_LOBBY_CHAT, session, buffer, numOfBytes);
		};
	}

	static bool HandlePacket(shared_ptr<GameSession>& session, BYTE* buffer, int numOfBytes)
	{
		auto header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->type](session, buffer, numOfBytes);
	}

	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_VERIFY_TOKEN& packet)
	{
		return MakeSendBuffer(packet, PKT_S_VERIFY_TOKEN);
	}

	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_LOBBY_CHAT& packet)
	{
		return MakeSendBuffer(packet, PKT_S_LOBBY_CHAT);
	}

private:
	template <typename PacketMessage, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, shared_ptr<GameSession>& session, BYTE* buffer, int numOfBytes)
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
