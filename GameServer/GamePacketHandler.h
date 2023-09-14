#pragma once
#include "Packet.pb.h"

struct PacketHeader
{
	uint16_t size;
	uint16_t type;
};

const int PacketHeaderSize = sizeof(PacketHeader);

/*
 *	Handler
 */

bool Handle_Invalid(shared_ptr<class GameSession> session, BYTE* bufer, int numOfBytes);
bool Handle_C_JOIN(shared_ptr<class GameSession> session, protocol::C_JOIN& packet);
bool Handle_C_LEAVE(shared_ptr<class GameSession> session, protocol::C_LEAVE& packet);
bool Handle_C_MOVE(shared_ptr<class GameSession> session, protocol::C_MOVE& packet);

/*
 *	GamePacketHandler
 */
using PacketHandler = function<bool(shared_ptr<class GameSession>, BYTE*, int)>;

class GamePacketHandler
{
	enum
	{
		HANDLER_SIZE = 10000
	};

public:
	static void Init();

	static bool HandlePacket(shared_ptr<class GameSession> session, BYTE* buffer, int numOfBytes);

	static shared_ptr<SendBuffer> MakeSendBuffer(protocol::S_JOIN& packet);

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
