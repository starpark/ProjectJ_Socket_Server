#include "pch.h"
#include "ServerPacketHandler.h"

#include "Service.h"

/*
 *	Handler
 */

bool HandleInvalid(shared_ptr<GameSession> session, BYTE* bufer, int numOfBytes)
{
	// TODO Log
	return false;
}


/*
 *	GamePacketHandler
 */

void ServerPacketHandler::Init()
{
	for (int i = 0; i < HANDLER_SIZE; i++)
	{
		handlers_[i] = HandleInvalid;
	}
	//BindHandler<protocol::C_Move>(protocol::PT_S_MOVE, Handle_S_Move);
}

bool ServerPacketHandler::HandlePacket(shared_ptr<GameSession> session, BYTE* buffer, int numOfBytes)
{
	auto header = reinterpret_cast<PacketHeader*>(buffer);
	if (header->type >= HANDLER_SIZE)
	{
		return handlers_[0](session, buffer, numOfBytes);
	}
	return handlers_[header->type](session, buffer, numOfBytes);
}

template <typename PacketMessage, typename HandlerFunc>
void ServerPacketHandler::BindHandler(protocol::PacketType type, HandlerFunc handler)
{
	handlers_[type] = [=](shared_ptr<class GameSession> session, BYTE* buffer, int numOfBytes)
	{
		return Handler<PacketMessage>(handler, session, buffer, numOfBytes);
	};
}

template <typename PacketMessage, typename ProcessFunc>
bool ServerPacketHandler::Handler(ProcessFunc func, shared_ptr<GameSession> session, BYTE* buffer, int numOfBytes)
{
	PacketMessage packet;
	if (packet.ParseFromArray(buffer + PacketHeaderSize, numOfBytes - PacketHeaderSize) == false)
	{
		return false;
	}

	return func(session, packet);
}

template <typename PacketMessage>
shared_ptr<SendBuffer> ServerPacketHandler::MakeSendBuffer(PacketMessage& packet, protocol::PacketType type)
{
	const uint32_t dataSize = static_cast<uint32_t>(packet.ByteSizeLong());
	const uint32_t packetSize = dataSize + sizeof(PacketHeader);

	auto sendBuffer = GSendBufferManager->GetSendBuffer(packetSize);
	auto header = reinterpret_cast<PacketHeader*>(sendBuffer->GetBuffer());
	header->size = packetSize;
	header->type = type;

	ASSERT_CRASH(packet.SerializeToArray(&header[1], dataSize));
	sendBuffer->Close(packetSize);

	return sendBuffer;
}


PacketHandler ServerPacketHandler::handlers_[HANDLER_SIZE];
