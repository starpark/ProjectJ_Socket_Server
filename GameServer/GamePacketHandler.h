#pragma once
#include "Message.pb.h"

struct PacketHeader
{
	uint16_t size;
	uint16_t type;
};

const int PacketHeaderSize = sizeof(PacketHeader);

/*
 *	Handler
 */

bool Handle_Invalid(shared_ptr<class GameSession>& session, BYTE* bufer, int numOfBytes);
bool Handle_C_VERIFY_TOKEN(shared_ptr<GameSession>& session, ProjectJ::C_VERIFY_TOKEN& packet);

/*
 *	GamePacketHandler
 */
using PacketHandler = function<bool(shared_ptr<GameSession>&, BYTE*, int)>;

class GamePacketHandler
{
	enum
	{
		HANDLER_SIZE = 10000
	};

public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_SIZE; i++)
		{
			handlers_[i] = Handle_Invalid;
		}
		BindHandler<ProjectJ::C_VERIFY_TOKEN>(ProjectJ::PT_C_VERIFY_TOKEN, Handle_C_VERIFY_TOKEN);
	}

	static bool HandlePacket(shared_ptr<GameSession>& session, BYTE* buffer, int numOfBytes)
	{
		auto header = reinterpret_cast<PacketHeader*>(buffer);
		return handlers_[header->type](session, buffer, numOfBytes);
	}


	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::S_VERIFY_TOKEN& packet)
	{
		return MakeSendBuffer(packet, ProjectJ::PT_S_VERIFY_TOKEN);
	}

private:
	template <typename PacketMessage, typename HandlerFunc>
	static void BindHandler(uint16_t type, HandlerFunc handler)
	{
		handlers_[type] = [=](shared_ptr<class GameSession> session, BYTE* buffer, int numOfBytes)
		{
			return Handler<PacketMessage>(handler, session, buffer, numOfBytes);
		};
	}

	template <typename PacketMessage, typename ProcessFunc>
	static bool Handler(ProcessFunc func, shared_ptr<GameSession>& session, BYTE* buffer, int numOfBytes)
	{
		PacketMessage packet;
		if (packet.ParseFromArray(buffer + PacketHeaderSize, numOfBytes - PacketHeaderSize) == false)
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

private:
	static PacketHandler handlers_[HANDLER_SIZE];
};
