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
	ProjectJ::S_ROOM_INFO,
	ProjectJ::S_ROOM_LEAVE,
	ProjectJ::S_ROOM_OTHER_ENTER,
	ProjectJ::S_ROOM_OTHER_LEAVE,
	ProjectJ::S_ROOM_READY,
	ProjectJ::S_ROOM_CHAT,
	ProjectJ::S_ROOM_STANDBY_MATCH,
	ProjectJ::S_ROOM_START_MATCH,
	ProjectJ::S_MATCH_ALL_READY_TO_RECEIVE,
	ProjectJ::S_MATCH_ITEM_GENERATED,
	ProjectJ::S_MATCH_START,
	ProjectJ::S_MATCH_INFO,
	ProjectJ::S_MATCH_END,
	ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP,
	ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE,
	ProjectJ::S_MATCH_ITEM_SOMEONE_DROP,
	ProjectJ::S_MATCH_SCALE_ON_CHANGED,
	ProjectJ::S_MATCH_CHASER_ATTACK,
	ProjectJ::S_MATCH_CHASER_HIT,
	ProjectJ::S_MATCH_FUGITIVE_ESCAPE,
	ProjectJ::S_MATCH_CHASER_INSTALL_CCTV,
	ProjectJ::S_MATCH_LEAVE
>;

// 패킷 래핑 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: 2023-11-28 자동 생성
class PROJECTJ_API JPackets
{
public:
	uint16 Type = 0;
	JPacketTypes Packet;
};