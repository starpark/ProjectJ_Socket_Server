#include "pch.h"
#include "GamePacketHandler.h"

#include "GameSession.h"
#include "Service.h"

/*
 *	Handler
 */

bool Handle_Invalid(shared_ptr<GameSession> session, BYTE* bufer, int numOfBytes)
{
	// TODO Log
	return false;
}

bool Handle_C_JOIN(shared_ptr<GameSession> session, protocol::C_JOIN& packet)
{
	static atomic<int> id = 0;

	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"Handle_C_JOIN\n");

	return true;
}

bool Handle_C_LEAVE(shared_ptr<GameSession> session, protocol::C_LEAVE& packet)
{
	GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"count %d\n", packet.id());
	return true;
}

bool Handle_C_MOVE(shared_ptr<GameSession> session, protocol::C_MOVE& packet)
{
	auto ownerService = session->GetOwnerService();

	protocol::S_MOVE pkt;
	pkt.set_id(packet.id());
	protocol::Vector vector = pkt.vector();
	vector = packet.vector();


	//ownerService->BroadCastWithoutSelf(session, )

	return true;
}


/*
 *	GamePacketHandler
 */

void GamePacketHandler::Init()
{
	for (int i = 0; i < HANDLER_SIZE; i++)
	{
		handlers_[i] = Handle_Invalid;
	}
	BindHandler<protocol::C_JOIN>(protocol::PT_C_JOIN, Handle_C_JOIN);
	BindHandler<protocol::C_LEAVE>(protocol::PT_C_LEAVE, Handle_C_LEAVE);
	BindHandler<protocol::C_MOVE>(protocol::PT_C_MOVE, Handle_C_MOVE);
}

bool GamePacketHandler::HandlePacket(shared_ptr<GameSession> session, BYTE* buffer, int numOfBytes)
{
	auto header = reinterpret_cast<PacketHeader*>(buffer);
	return handlers_[header->type](session, buffer, numOfBytes);
}

shared_ptr<SendBuffer> GamePacketHandler::MakeSendBuffer(protocol::S_JOIN& packet)
{
	return MakeSendBuffer(packet, protocol::PT_S_JOIN);
}

shared_ptr<SendBuffer> GamePacketHandler::MakeSendBuffer(protocol::S_LEAVE& packet)
{
	return MakeSendBuffer(packet, protocol::PT_S_LEAVE);
}

template <typename PacketMessage, typename HandlerFunc>
void GamePacketHandler::BindHandler(protocol::PacketType type, HandlerFunc handler)
{
	handlers_[type] = [=](shared_ptr<class GameSession> session, BYTE* buffer, int numOfBytes)
	{
		return Handler<PacketMessage>(handler, session, buffer, numOfBytes);
	};
}

template <typename PacketMessage, typename ProcessFunc>
bool GamePacketHandler::Handler(ProcessFunc func, shared_ptr<GameSession> session, BYTE* buffer, int numOfBytes)
{
	PacketMessage packet;
	if (packet.ParseFromArray(buffer + PacketHeaderSize, numOfBytes - PacketHeaderSize) == false)
	{
		return false;
	}

	return func(session, packet);
}

template <typename PacketMessage>
shared_ptr<SendBuffer> GamePacketHandler::MakeSendBuffer(PacketMessage& packet, protocol::PacketType type)
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


PacketHandler GamePacketHandler::handlers_[HANDLER_SIZE];
