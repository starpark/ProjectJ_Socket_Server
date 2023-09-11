#pragma once
#include "Packet.pb.h"

struct PacketHeader
{
	uint32_t size;
	protocol::PacketType type;
};

const int PacketHeaderSize = sizeof(PacketHeader);

/*
 *	Handler
 */

bool HandleInvalid(shared_ptr<class GameSession> session, BYTE* bufer, int numOfBytes);


/*
 *	GamePacketHandler
 */
using PacketHandler = function<bool(shared_ptr<class GameSession>, BYTE*, int)>;

class ServerPacketHandler
{
	enum
	{
		HANDLER_SIZE = 10000
	};

public:
	static void Init();

	static bool HandlePacket(shared_ptr<class GameSession> session, BYTE* buffer, int numOfBytes);

public:
	static shared_ptr<SendBuffer> MakeSendBuffer_S_Join(protocol::S_Join& packet)
	{
	}

private:
	template <typename PacketMessage, typename HandlerFunc>
	static void BindHandler(protocol::PacketType type, HandlerFunc handler);

	template <typename PacketMessage, typename ProcessFunc>
	static bool Handler(ProcessFunc func, shared_ptr<class GameSession> session, BYTE* buffer, int numOfBytes);

	template <typename PacketMessage>
	static shared_ptr<SendBuffer> MakeSendBuffer(PacketMessage& packet, protocol::PacketType type);

private:
	static PacketHandler handlers_[HANDLER_SIZE];
};
