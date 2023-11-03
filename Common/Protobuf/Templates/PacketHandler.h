#pragma once
#include "Message.pb.h"

using PacketHandlerFunc = function<bool(shared_ptr<SessionBase>&, BYTE*, int)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16_t
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

bool Handle_INVALID(shared_ptr<SessionBase>& session, BYTE* bufer, int numOfBytes);
{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(shared_ptr<SessionBase>& session, ProjectJ::{{pkt.name}}& packet);
{%- endfor %}


// 소켓 수신 데이터 처리 및 송신 버퍼 생성 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: {{date}} 자동 생성
class {{output}}
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
{%- for pkt in parser.recv_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](shared_ptr<SessionBase> session, BYTE* buffer, int numOfBytes) {return HandlePacket<ProjectJ::{{pkt.name}}>(Handle_{{pkt.name}}, session, buffer, numOfBytes);};
{%- endfor %}
	}

	static bool HandlePacket(shared_ptr<SessionBase>& session, BYTE* buffer, int numOfBytes)
	{
		auto header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->type](session, buffer, numOfBytes);
	}

{%- for pkt in parser.send_pkt %}
	static shared_ptr<SendBuffer> MakeSendBuffer(ProjectJ::{{pkt.name}}& packet) {return MakeSendBuffer(packet, PKT_{{pkt.name}});}
{%- endfor %}

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
