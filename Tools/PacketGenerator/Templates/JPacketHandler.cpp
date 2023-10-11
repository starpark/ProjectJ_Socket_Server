// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/JPacketHandler.h"


PacketProcessorFunc GPacketProcessor[UINT16_MAX];
PacketHandlerFunc GPacketHandler[UINT16_MAX];
FPacket_INVALID UJPacketHandler::Packet_INVALID_Delegate;
{%- for pkt in parser.send_pkt %}
FPacket_{{pkt.name}} UJPacketHandler::Packet_{{pkt.name}}_Delegate;
{%- endfor %}

bool Handle_INVALID(UWorld* World, const TSharedPtr<JPackets>& Packet)
{
	bool Result = UJPacketHandler::Packet_INVALID_Delegate.ExecuteIfBound(World);
	return false;
}

{%- for pkt in parser.send_pkt %}
bool Handle_{{pkt.name}}(UWorld* World, ProjectJ::{{pkt.name}}& Packet)
{
	if (UJPacketHandler::Packet_{{pkt.name}}_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_{{pkt.name}}_Delegate.Execute(World, Packet);
	}

	return false;
}
{%- endfor %}