// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Network/Protocol/Message.pb.h"

using JPacketTypes = TVariant<
	ProjectJ::S_VERIFY_TOKEN,
	ProjectJ::S_LOBBY_CHAT,
	ProjectJ::S_LOBBY_REFRESH_ROOM,
	ProjectJ::S_LOBBY_CREATE_ROOM,
	ProjectJ::S_LOBBY_ENTER_ROOM,
	ProjectJ::S_ROOM_LEAVE,
	ProjectJ::S_ROOM_OTHER_ENTER,
	ProjectJ::S_ROOM_OTHER_LEAVE,
	ProjectJ::S_ROOM_READY,
	ProjectJ::S_ROOM_CHAT,
	ProjectJ::S_MATCH_INIT_GENERATED_ITEMS,
	ProjectJ::S_MATCH_ITEM_PICKUP,
	ProjectJ::S_MATCH_ITEM_MOVE,
	ProjectJ::S_MATCH_ITEM_DROP
>;

// 패킷 래핑 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: 2023-11-04 자동 생성
class PROJECTJ_API JPackets
{
public:
	uint16 Type = 0;
	JPacketTypes Packet;
};