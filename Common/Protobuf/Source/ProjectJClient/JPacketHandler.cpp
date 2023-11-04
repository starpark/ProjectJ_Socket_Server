// Fill out your copyright notice in the Description page of Project Settings.
#include "Network/JPacketHandler.h"

PacketProcessorFunc GPacketProcessor[UINT16_MAX];
PacketHandlerFunc GPacketHandler[UINT16_MAX];
FPacket_INVALID UJPacketHandler::Packet_INVALID_Delegate;
FPacket_S_VERIFY_TOKEN UJPacketHandler::Packet_S_VERIFY_TOKEN_Delegate;
FPacket_S_LOBBY_CHAT UJPacketHandler::Packet_S_LOBBY_CHAT_Delegate;
FPacket_S_LOBBY_REFRESH_ROOM UJPacketHandler::Packet_S_LOBBY_REFRESH_ROOM_Delegate;
FPacket_S_LOBBY_CREATE_ROOM UJPacketHandler::Packet_S_LOBBY_CREATE_ROOM_Delegate;
FPacket_S_LOBBY_ENTER_ROOM UJPacketHandler::Packet_S_LOBBY_ENTER_ROOM_Delegate;
FPacket_S_ROOM_LEAVE UJPacketHandler::Packet_S_ROOM_LEAVE_Delegate;
FPacket_S_ROOM_OTHER_ENTER UJPacketHandler::Packet_S_ROOM_OTHER_ENTER_Delegate;
FPacket_S_ROOM_OTHER_LEAVE UJPacketHandler::Packet_S_ROOM_OTHER_LEAVE_Delegate;
FPacket_S_ROOM_READY UJPacketHandler::Packet_S_ROOM_READY_Delegate;
FPacket_S_ROOM_CHAT UJPacketHandler::Packet_S_ROOM_CHAT_Delegate;
FPacket_S_MATCH_INIT_GENERATED_ITEMS UJPacketHandler::Packet_S_MATCH_INIT_GENERATED_ITEMS_Delegate;
FPacket_S_MATCH_ITEM_PICKUP UJPacketHandler::Packet_S_MATCH_ITEM_PICKUP_Delegate;
FPacket_S_MATCH_ITEM_MOVE UJPacketHandler::Packet_S_MATCH_ITEM_MOVE_Delegate;
FPacket_S_MATCH_ITEM_DROP UJPacketHandler::Packet_S_MATCH_ITEM_DROP_Delegate;

bool Handle_INVALID(UWorld* World, const TSharedPtr<JPackets>& Packet, float DeltaSeconds)
{
	bool Result = UJPacketHandler::Packet_INVALID_Delegate.ExecuteIfBound(World, DeltaSeconds);
	return false;
}
bool Handle_S_VERIFY_TOKEN(UWorld* World, ProjectJ::S_VERIFY_TOKEN& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_VERIFY_TOKEN_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_VERIFY_TOKEN_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_LOBBY_CHAT(UWorld* World, ProjectJ::S_LOBBY_CHAT& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_LOBBY_CHAT_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_LOBBY_CHAT_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_LOBBY_REFRESH_ROOM(UWorld* World, ProjectJ::S_LOBBY_REFRESH_ROOM& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_LOBBY_REFRESH_ROOM_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_LOBBY_REFRESH_ROOM_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_LOBBY_CREATE_ROOM(UWorld* World, ProjectJ::S_LOBBY_CREATE_ROOM& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_LOBBY_CREATE_ROOM_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_LOBBY_CREATE_ROOM_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_LOBBY_ENTER_ROOM(UWorld* World, ProjectJ::S_LOBBY_ENTER_ROOM& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_LOBBY_ENTER_ROOM_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_LOBBY_ENTER_ROOM_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_ROOM_LEAVE(UWorld* World, ProjectJ::S_ROOM_LEAVE& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_LEAVE_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_LEAVE_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_ROOM_OTHER_ENTER(UWorld* World, ProjectJ::S_ROOM_OTHER_ENTER& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_OTHER_ENTER_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_OTHER_ENTER_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_ROOM_OTHER_LEAVE(UWorld* World, ProjectJ::S_ROOM_OTHER_LEAVE& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_OTHER_LEAVE_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_OTHER_LEAVE_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_ROOM_READY(UWorld* World, ProjectJ::S_ROOM_READY& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_READY_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_READY_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_ROOM_CHAT(UWorld* World, ProjectJ::S_ROOM_CHAT& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_CHAT_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_CHAT_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_INIT_GENERATED_ITEMS(UWorld* World, ProjectJ::S_MATCH_INIT_GENERATED_ITEMS& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_INIT_GENERATED_ITEMS_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_INIT_GENERATED_ITEMS_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ITEM_PICKUP(UWorld* World, ProjectJ::S_MATCH_ITEM_PICKUP& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ITEM_PICKUP_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ITEM_PICKUP_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ITEM_MOVE(UWorld* World, ProjectJ::S_MATCH_ITEM_MOVE& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ITEM_MOVE_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ITEM_MOVE_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ITEM_DROP(UWorld* World, ProjectJ::S_MATCH_ITEM_DROP& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ITEM_DROP_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ITEM_DROP_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}