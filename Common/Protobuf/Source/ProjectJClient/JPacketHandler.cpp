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
FPacket_S_ROOM_INFO UJPacketHandler::Packet_S_ROOM_INFO_Delegate;
FPacket_S_ROOM_LEAVE UJPacketHandler::Packet_S_ROOM_LEAVE_Delegate;
FPacket_S_ROOM_OTHER_ENTER UJPacketHandler::Packet_S_ROOM_OTHER_ENTER_Delegate;
FPacket_S_ROOM_OTHER_LEAVE UJPacketHandler::Packet_S_ROOM_OTHER_LEAVE_Delegate;
FPacket_S_ROOM_READY UJPacketHandler::Packet_S_ROOM_READY_Delegate;
FPacket_S_ROOM_CHAT UJPacketHandler::Packet_S_ROOM_CHAT_Delegate;
FPacket_S_ROOM_STANDBY_MATCH UJPacketHandler::Packet_S_ROOM_STANDBY_MATCH_Delegate;
FPacket_S_ROOM_START_MATCH UJPacketHandler::Packet_S_ROOM_START_MATCH_Delegate;
FPacket_S_MATCH_ALL_READY_TO_RECEIVE UJPacketHandler::Packet_S_MATCH_ALL_READY_TO_RECEIVE_Delegate;
FPacket_S_MATCH_ITEM_GENERATED UJPacketHandler::Packet_S_MATCH_ITEM_GENERATED_Delegate;
FPacket_S_MATCH_START UJPacketHandler::Packet_S_MATCH_START_Delegate;
FPacket_S_MATCH_INFO UJPacketHandler::Packet_S_MATCH_INFO_Delegate;
FPacket_S_MATCH_END UJPacketHandler::Packet_S_MATCH_END_Delegate;
FPacket_S_MATCH_ITEM_SOMEONE_PICKUP UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_PICKUP_Delegate;
FPacket_S_MATCH_ITEM_SOMEONE_MOVE UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_MOVE_Delegate;
FPacket_S_MATCH_ITEM_SOMEONE_DROP UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_DROP_Delegate;
FPacket_S_MATCH_SCALE_ON_CHANGED UJPacketHandler::Packet_S_MATCH_SCALE_ON_CHANGED_Delegate;
FPacket_S_MATCH_CHASER_ATTACK UJPacketHandler::Packet_S_MATCH_CHASER_ATTACK_Delegate;
FPacket_S_MATCH_CHASER_HIT UJPacketHandler::Packet_S_MATCH_CHASER_HIT_Delegate;
FPacket_S_MATCH_CHASER_INSTALL_CCTV UJPacketHandler::Packet_S_MATCH_CHASER_INSTALL_CCTV_Delegate;
FPacket_S_MATCH_LEAVE UJPacketHandler::Packet_S_MATCH_LEAVE_Delegate;

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
bool Handle_S_ROOM_INFO(UWorld* World, ProjectJ::S_ROOM_INFO& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_INFO_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_INFO_Delegate.Execute(World, Packet, DeltaSeconds);
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
bool Handle_S_ROOM_STANDBY_MATCH(UWorld* World, ProjectJ::S_ROOM_STANDBY_MATCH& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_STANDBY_MATCH_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_STANDBY_MATCH_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_ROOM_START_MATCH(UWorld* World, ProjectJ::S_ROOM_START_MATCH& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_ROOM_START_MATCH_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_ROOM_START_MATCH_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ALL_READY_TO_RECEIVE(UWorld* World, ProjectJ::S_MATCH_ALL_READY_TO_RECEIVE& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ALL_READY_TO_RECEIVE_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ALL_READY_TO_RECEIVE_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ITEM_GENERATED(UWorld* World, ProjectJ::S_MATCH_ITEM_GENERATED& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ITEM_GENERATED_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ITEM_GENERATED_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_START(UWorld* World, ProjectJ::S_MATCH_START& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_START_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_START_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_INFO(UWorld* World, ProjectJ::S_MATCH_INFO& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_INFO_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_INFO_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_END(UWorld* World, ProjectJ::S_MATCH_END& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_END_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_END_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ITEM_SOMEONE_PICKUP(UWorld* World, ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_PICKUP_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_PICKUP_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ITEM_SOMEONE_MOVE(UWorld* World, ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_MOVE_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_MOVE_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_ITEM_SOMEONE_DROP(UWorld* World, ProjectJ::S_MATCH_ITEM_SOMEONE_DROP& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_DROP_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_ITEM_SOMEONE_DROP_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_SCALE_ON_CHANGED(UWorld* World, ProjectJ::S_MATCH_SCALE_ON_CHANGED& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_SCALE_ON_CHANGED_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_SCALE_ON_CHANGED_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_CHASER_ATTACK(UWorld* World, ProjectJ::S_MATCH_CHASER_ATTACK& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_CHASER_ATTACK_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_CHASER_ATTACK_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_CHASER_HIT(UWorld* World, ProjectJ::S_MATCH_CHASER_HIT& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_CHASER_HIT_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_CHASER_HIT_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_CHASER_INSTALL_CCTV(UWorld* World, ProjectJ::S_MATCH_CHASER_INSTALL_CCTV& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_CHASER_INSTALL_CCTV_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_CHASER_INSTALL_CCTV_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}
bool Handle_S_MATCH_LEAVE(UWorld* World, ProjectJ::S_MATCH_LEAVE& Packet, float DeltaSeconds)
{
	if (UJPacketHandler::Packet_S_MATCH_LEAVE_Delegate.IsBound())
	{
		return UJPacketHandler::Packet_S_MATCH_LEAVE_Delegate.Execute(World, Packet, DeltaSeconds);
	}

	return false;
}