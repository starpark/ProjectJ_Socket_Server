// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/NoExportTypes.h"
#include "Network/Protocol/JPackets.h"
#include "Network/JSendBuffer.h"
#include "Network/JGameInstance.h"
#include "Network/JPacketHeader.h"
#include "Network/JGameInstanceSubsystem.h"

enum : uint16
{
	PKT_C_VERIFY_TOKEN = 1000,
	PKT_S_VERIFY_TOKEN = 1001,
	PKT_C_LOBBY_CHAT = 1002,
	PKT_S_LOBBY_CHAT = 1003,
	PKT_C_LOBBY_REFRESH_ROOM = 1004,
	PKT_S_LOBBY_REFRESH_ROOM = 1005,
	PKT_C_LOBBY_CREATE_ROOM = 1006,
	PKT_S_LOBBY_CREATE_ROOM = 1007,
	PKT_C_LOBBY_ENTER_ROOM = 1008,
	PKT_S_LOBBY_ENTER_ROOM = 1009,
	PKT_C_ROOM_READY_TO_RECEIVE = 1010,
	PKT_S_ROOM_INFO = 1011,
	PKT_C_ROOM_LEAVE = 1012,
	PKT_S_ROOM_LEAVE = 1013,
	PKT_S_ROOM_OTHER_ENTER = 1014,
	PKT_S_ROOM_OTHER_LEAVE = 1015,
	PKT_C_ROOM_READY = 1016,
	PKT_S_ROOM_READY = 1017,
	PKT_C_ROOM_CHAT = 1018,
	PKT_S_ROOM_CHAT = 1019,
	PKT_S_ROOM_STANDBY_MATCH = 1020,
	PKT_S_ROOM_START_MATCH = 1021,
	PKT_C_MATCH_READY_TO_RECEIVE = 1022,
	PKT_S_MATCH_ALL_READY_TO_RECIEVE = 1023,
	PKT_S_MATCH_ITEM_GENERATED = 1024,
	PKT_C_MATCH_READY_TO_START = 1025,
	PKT_S_MATCH_START = 1026,
	PKT_C_MATCH_INFO = 1027,
	PKT_S_MATCH_INFO = 1028,
	PKT_S_MATCH_END = 1029,
	PKT_C_MATCH_ITEM_PICKUP = 1030,
	PKT_C_MATCH_ITEM_MOVE = 1031,
	PKT_C_MATCH_ITEM_DROP = 1032,
	PKT_S_MATCH_ITEM_SOMEONE_PICKUP = 1033,
	PKT_S_MATCH_ITEM_SOMEONE_MOVE = 1034,
	PKT_S_MATCH_ITEM_SOMEONE_DROP = 1035,
	PKT_S_MATCH_SCALE_ON_CHANGED = 1036,
	PKT_C_MATCH_CHASER_ATTACK = 1037,
	PKT_S_MATCH_CHASER_ATTACK = 1038,
	PKT_C_MATCH_CHASER_HIT = 1039,
	PKT_S_MATCH_CHASER_HIT = 1040,
};

// RecvThread 전용 패킷 가공 함수
using PacketProcessorFunc = TFunction<bool(UWorld*, const TSharedPtr<JPackets>&, float DeltaSeconds)>;
extern PacketProcessorFunc GPacketProcessor[UINT16_MAX];

// GameThread 전용 패킷 처리 함수
using PacketHandlerFunc = TFunction<TSharedPtr<JPackets>(uint16, uint8*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 패킷 처리 함수
bool Handle_INVALID(UWorld* World, const TSharedPtr<JPackets>& Packet, float DeltaSeconds);
DECLARE_DELEGATE_TwoParams(FPacket_INVALID, UWorld*, float);
bool Handle_S_VERIFY_TOKEN(UWorld* World, ProjectJ::S_VERIFY_TOKEN& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_VERIFY_TOKEN, UWorld*, ProjectJ::S_VERIFY_TOKEN&, float);
bool Handle_S_LOBBY_CHAT(UWorld* World, ProjectJ::S_LOBBY_CHAT& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_LOBBY_CHAT, UWorld*, ProjectJ::S_LOBBY_CHAT&, float);
bool Handle_S_LOBBY_REFRESH_ROOM(UWorld* World, ProjectJ::S_LOBBY_REFRESH_ROOM& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_LOBBY_REFRESH_ROOM, UWorld*, ProjectJ::S_LOBBY_REFRESH_ROOM&, float);
bool Handle_S_LOBBY_CREATE_ROOM(UWorld* World, ProjectJ::S_LOBBY_CREATE_ROOM& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_LOBBY_CREATE_ROOM, UWorld*, ProjectJ::S_LOBBY_CREATE_ROOM&, float);
bool Handle_S_LOBBY_ENTER_ROOM(UWorld* World, ProjectJ::S_LOBBY_ENTER_ROOM& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_LOBBY_ENTER_ROOM, UWorld*, ProjectJ::S_LOBBY_ENTER_ROOM&, float);
bool Handle_S_ROOM_INFO(UWorld* World, ProjectJ::S_ROOM_INFO& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_INFO, UWorld*, ProjectJ::S_ROOM_INFO&, float);
bool Handle_S_ROOM_LEAVE(UWorld* World, ProjectJ::S_ROOM_LEAVE& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_LEAVE, UWorld*, ProjectJ::S_ROOM_LEAVE&, float);
bool Handle_S_ROOM_OTHER_ENTER(UWorld* World, ProjectJ::S_ROOM_OTHER_ENTER& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_OTHER_ENTER, UWorld*, ProjectJ::S_ROOM_OTHER_ENTER&, float);
bool Handle_S_ROOM_OTHER_LEAVE(UWorld* World, ProjectJ::S_ROOM_OTHER_LEAVE& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_OTHER_LEAVE, UWorld*, ProjectJ::S_ROOM_OTHER_LEAVE&, float);
bool Handle_S_ROOM_READY(UWorld* World, ProjectJ::S_ROOM_READY& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_READY, UWorld*, ProjectJ::S_ROOM_READY&, float);
bool Handle_S_ROOM_CHAT(UWorld* World, ProjectJ::S_ROOM_CHAT& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_CHAT, UWorld*, ProjectJ::S_ROOM_CHAT&, float);
bool Handle_S_ROOM_STANDBY_MATCH(UWorld* World, ProjectJ::S_ROOM_STANDBY_MATCH& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_STANDBY_MATCH, UWorld*, ProjectJ::S_ROOM_STANDBY_MATCH&, float);
bool Handle_S_ROOM_START_MATCH(UWorld* World, ProjectJ::S_ROOM_START_MATCH& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_ROOM_START_MATCH, UWorld*, ProjectJ::S_ROOM_START_MATCH&, float);
bool Handle_S_MATCH_ALL_READY_TO_RECIEVE(UWorld* World, ProjectJ::S_MATCH_ALL_READY_TO_RECIEVE& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_ALL_READY_TO_RECIEVE, UWorld*, ProjectJ::S_MATCH_ALL_READY_TO_RECIEVE&, float);
bool Handle_S_MATCH_ITEM_GENERATED(UWorld* World, ProjectJ::S_MATCH_ITEM_GENERATED& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_ITEM_GENERATED, UWorld*, ProjectJ::S_MATCH_ITEM_GENERATED&, float);
bool Handle_S_MATCH_START(UWorld* World, ProjectJ::S_MATCH_START& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_START, UWorld*, ProjectJ::S_MATCH_START&, float);
bool Handle_S_MATCH_INFO(UWorld* World, ProjectJ::S_MATCH_INFO& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_INFO, UWorld*, ProjectJ::S_MATCH_INFO&, float);
bool Handle_S_MATCH_END(UWorld* World, ProjectJ::S_MATCH_END& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_END, UWorld*, ProjectJ::S_MATCH_END&, float);
bool Handle_S_MATCH_ITEM_SOMEONE_PICKUP(UWorld* World, ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_ITEM_SOMEONE_PICKUP, UWorld*, ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP&, float);
bool Handle_S_MATCH_ITEM_SOMEONE_MOVE(UWorld* World, ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_ITEM_SOMEONE_MOVE, UWorld*, ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE&, float);
bool Handle_S_MATCH_ITEM_SOMEONE_DROP(UWorld* World, ProjectJ::S_MATCH_ITEM_SOMEONE_DROP& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_ITEM_SOMEONE_DROP, UWorld*, ProjectJ::S_MATCH_ITEM_SOMEONE_DROP&, float);
bool Handle_S_MATCH_SCALE_ON_CHANGED(UWorld* World, ProjectJ::S_MATCH_SCALE_ON_CHANGED& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_SCALE_ON_CHANGED, UWorld*, ProjectJ::S_MATCH_SCALE_ON_CHANGED&, float);
bool Handle_S_MATCH_CHASER_ATTACK(UWorld* World, ProjectJ::S_MATCH_CHASER_ATTACK& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_CHASER_ATTACK, UWorld*, ProjectJ::S_MATCH_CHASER_ATTACK&, float);
bool Handle_S_MATCH_CHASER_HIT(UWorld* World, ProjectJ::S_MATCH_CHASER_HIT& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_S_MATCH_CHASER_HIT, UWorld*, ProjectJ::S_MATCH_CHASER_HIT&, float);

// 소켓 수신 데이터 처리 및 송신 버퍼 생성 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: 2023-11-18 자동 생성
class PROJECTJ_API UJPacketHandler : public UObject
{
public:
	// 처리 함수 배열 초기화 함수
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketProcessor[i] = Handle_INVALID;
			GPacketHandler[i] = [](uint16 TypeCode, uint8* Buffer, int32 Size)
			{
				return nullptr;
			};
		}

		// Processor 바인딩
		GPacketProcessor[PKT_S_VERIFY_TOKEN] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_VERIFY_TOKEN>(Handle_S_VERIFY_TOKEN, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_LOBBY_CHAT] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_LOBBY_CHAT>(Handle_S_LOBBY_CHAT, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_LOBBY_REFRESH_ROOM] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_LOBBY_REFRESH_ROOM>(Handle_S_LOBBY_REFRESH_ROOM, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_LOBBY_CREATE_ROOM] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_LOBBY_CREATE_ROOM>(Handle_S_LOBBY_CREATE_ROOM, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_LOBBY_ENTER_ROOM] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_LOBBY_ENTER_ROOM>(Handle_S_LOBBY_ENTER_ROOM, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_INFO] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_INFO>(Handle_S_ROOM_INFO, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_LEAVE] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_LEAVE>(Handle_S_ROOM_LEAVE, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_OTHER_ENTER] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_OTHER_ENTER>(Handle_S_ROOM_OTHER_ENTER, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_OTHER_LEAVE] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_OTHER_LEAVE>(Handle_S_ROOM_OTHER_LEAVE, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_READY] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_READY>(Handle_S_ROOM_READY, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_CHAT] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_CHAT>(Handle_S_ROOM_CHAT, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_STANDBY_MATCH] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_STANDBY_MATCH>(Handle_S_ROOM_STANDBY_MATCH, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_ROOM_START_MATCH] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_ROOM_START_MATCH>(Handle_S_ROOM_START_MATCH, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_ALL_READY_TO_RECIEVE] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_ALL_READY_TO_RECIEVE>(Handle_S_MATCH_ALL_READY_TO_RECIEVE, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_ITEM_GENERATED] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_ITEM_GENERATED>(Handle_S_MATCH_ITEM_GENERATED, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_START] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_START>(Handle_S_MATCH_START, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_INFO] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_INFO>(Handle_S_MATCH_INFO, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_END] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_END>(Handle_S_MATCH_END, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_ITEM_SOMEONE_PICKUP] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP>(Handle_S_MATCH_ITEM_SOMEONE_PICKUP, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_ITEM_SOMEONE_MOVE] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE>(Handle_S_MATCH_ITEM_SOMEONE_MOVE, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_ITEM_SOMEONE_DROP] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_ITEM_SOMEONE_DROP>(Handle_S_MATCH_ITEM_SOMEONE_DROP, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_SCALE_ON_CHANGED] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_SCALE_ON_CHANGED>(Handle_S_MATCH_SCALE_ON_CHANGED, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_CHASER_ATTACK] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_CHASER_ATTACK>(Handle_S_MATCH_CHASER_ATTACK, World, PacketPtr, DeltaSeconds);};
		GPacketProcessor[PKT_S_MATCH_CHASER_HIT] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::S_MATCH_CHASER_HIT>(Handle_S_MATCH_CHASER_HIT, World, PacketPtr, DeltaSeconds);};

		// Handler 바인딩
		GPacketHandler[PKT_S_VERIFY_TOKEN] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_VERIFY_TOKEN>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_LOBBY_CHAT] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_LOBBY_CHAT>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_LOBBY_REFRESH_ROOM] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_LOBBY_REFRESH_ROOM>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_LOBBY_CREATE_ROOM] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_LOBBY_CREATE_ROOM>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_LOBBY_ENTER_ROOM] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_LOBBY_ENTER_ROOM>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_INFO] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_INFO>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_LEAVE] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_LEAVE>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_OTHER_ENTER] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_OTHER_ENTER>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_OTHER_LEAVE] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_OTHER_LEAVE>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_READY] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_READY>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_CHAT] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_CHAT>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_STANDBY_MATCH] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_STANDBY_MATCH>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_ROOM_START_MATCH] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_ROOM_START_MATCH>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_ALL_READY_TO_RECIEVE] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_ALL_READY_TO_RECIEVE>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_ITEM_GENERATED] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_ITEM_GENERATED>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_START] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_START>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_INFO] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_INFO>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_END] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_END>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_ITEM_SOMEONE_PICKUP] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_ITEM_SOMEONE_PICKUP>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_ITEM_SOMEONE_MOVE] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_ITEM_SOMEONE_MOVE>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_ITEM_SOMEONE_DROP] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_ITEM_SOMEONE_DROP>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_SCALE_ON_CHANGED] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_SCALE_ON_CHANGED>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_CHASER_ATTACK] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_CHASER_ATTACK>(TypeCode, Buffer, Size);};
		GPacketHandler[PKT_S_MATCH_CHASER_HIT] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::S_MATCH_CHASER_HIT>(TypeCode, Buffer, Size);};
	}

	// 게임 스레드에서 가공된 데이터를 처리하는 함수
	// World: 현재 게임의 UWorld 객체 주소
	// 패킷 처리 성공 여부 반환
	static bool ProcessPacket(UWorld* World, float DeltaSeconds)
	{
		UJGameInstance* JGameInstance = Cast<UJGameInstance>(World->GetGameInstance());
		if (JGameInstance)
		{
			TSharedPtr<JPackets> Packet;
			if (JGameInstance->GetPacketQueue().Dequeue(Packet))
			{
				return GPacketProcessor[Packet->Type](World, Packet, DeltaSeconds);
			}
		}

		return false;
	}

	// 수신 스레드에서 수신 받은 데이터를 가공하는 함수
	// Buffer: 수신받은 데이터의 주소
	// Size: 수신받은 데이터의 크기
	// 가공된 데이터 객체를 반환
	static TSharedPtr<JPackets> HandlePacket(uint8* Buffer, int32 Size)
	{
		FJPacketHeader* Header = reinterpret_cast<FJPacketHeader*>(Buffer);
		return GPacketHandler[Header->Type](Header->Type, Buffer, Size);
	}

public:
	// C_VERIFY_TOKEN를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_VERIFY_TOKEN 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_VERIFY_TOKEN& Packet) {return MakeSendBuffer(Packet, PKT_C_VERIFY_TOKEN);}
	// C_LOBBY_CHAT를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_LOBBY_CHAT 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_LOBBY_CHAT& Packet) {return MakeSendBuffer(Packet, PKT_C_LOBBY_CHAT);}
	// C_LOBBY_REFRESH_ROOM를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_LOBBY_REFRESH_ROOM 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_LOBBY_REFRESH_ROOM& Packet) {return MakeSendBuffer(Packet, PKT_C_LOBBY_REFRESH_ROOM);}
	// C_LOBBY_CREATE_ROOM를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_LOBBY_CREATE_ROOM 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_LOBBY_CREATE_ROOM& Packet) {return MakeSendBuffer(Packet, PKT_C_LOBBY_CREATE_ROOM);}
	// C_LOBBY_ENTER_ROOM를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_LOBBY_ENTER_ROOM 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_LOBBY_ENTER_ROOM& Packet) {return MakeSendBuffer(Packet, PKT_C_LOBBY_ENTER_ROOM);}
	// C_ROOM_READY_TO_RECEIVE를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_ROOM_READY_TO_RECEIVE 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_ROOM_READY_TO_RECEIVE& Packet) {return MakeSendBuffer(Packet, PKT_C_ROOM_READY_TO_RECEIVE);}
	// C_ROOM_LEAVE를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_ROOM_LEAVE 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_ROOM_LEAVE& Packet) {return MakeSendBuffer(Packet, PKT_C_ROOM_LEAVE);}
	// C_ROOM_READY를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_ROOM_READY 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_ROOM_READY& Packet) {return MakeSendBuffer(Packet, PKT_C_ROOM_READY);}
	// C_ROOM_CHAT를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_ROOM_CHAT 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_ROOM_CHAT& Packet) {return MakeSendBuffer(Packet, PKT_C_ROOM_CHAT);}
	// C_MATCH_READY_TO_RECEIVE를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_READY_TO_RECEIVE 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_READY_TO_RECEIVE& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_READY_TO_RECEIVE);}
	// C_MATCH_READY_TO_START를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_READY_TO_START 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_READY_TO_START& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_READY_TO_START);}
	// C_MATCH_INFO를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_INFO 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_INFO& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_INFO);}
	// C_MATCH_ITEM_PICKUP를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_ITEM_PICKUP 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_ITEM_PICKUP& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_ITEM_PICKUP);}
	// C_MATCH_ITEM_MOVE를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_ITEM_MOVE 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_ITEM_MOVE& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_ITEM_MOVE);}
	// C_MATCH_ITEM_DROP를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_ITEM_DROP 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_ITEM_DROP& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_ITEM_DROP);}
	// C_MATCH_CHASER_ATTACK를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_CHASER_ATTACK 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_CHASER_ATTACK& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_CHASER_ATTACK);}
	// C_MATCH_CHASER_HIT를 직렬화한 FJSendBuffer 생성 함수
	// Packet: C_MATCH_CHASER_HIT 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::C_MATCH_CHASER_HIT& Packet) {return MakeSendBuffer(Packet, PKT_C_MATCH_CHASER_HIT);}

protected:
	// 게임 스레드에서 송신할 데이터를 담을 객체를 만드는 함수
	// Packet: 송신할 데이터를 담은 Protobuf 객체
	// PacketType: 송신할 데이터의 식별 번호
	// 바이트 스트림으로 직렬화된 객체의 스마트 포인터를 반환
	template <typename T>
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(T& Packet, uint16 PacketNumber)
	{
		TSharedPtr<FJSendBuffer> SendBuffer;

		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GEngine->GameViewport->GetWorld());
		if (GameInstance)
		{
			UJGameInstanceSubsystem* Subsystem = GameInstance->GetSubsystem<UJGameInstanceSubsystem>();

			if (Subsystem)
			{
				const uint16 DataSize = static_cast<uint16>(Packet.ByteSizeLong());
				const uint16 PacketSize = DataSize + sizeof(FJPacketHeader);

				SendBuffer = Subsystem->SendBufferManager->Open(PacketSize);
				FJPacketHeader* Header = reinterpret_cast<FJPacketHeader*>(SendBuffer->GetBuffer());
				Header->Size = PacketSize;
				Header->Type = PacketNumber;

				check(Packet.SerializeToArray(&Header[1], DataSize));

				SendBuffer->Close(PacketSize);
			}
		}

		return SendBuffer;
	}

	// 가공된 패킷 객체를 처리하는 함수
	// Init 함수에서 GPacketProcessor 함수 배열에 바인딩됨
	// Func: 처리 함수 포인터
	// World: 현재 게임의 UWorld 객체 주소
	// PendingPacket: 처리 대기중인 가공된 패킷 객체 참조
	// 패킷 처리 성공 여부
	template <typename PacketType, typename ProcessFunc>
	static bool ProcessPacket(ProcessFunc Func, UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds)
	{
		PacketType Packet = PacketPtr->Packet.Get<PacketType>();
		bool ProcessResult = Func(World, Packet, DeltaSeconds);
		return ProcessResult;
	}

	// 수신받은 데이터를 JPackets 객체로 가공하는 함수
	// TypeCode: 패킷 식별 번호
	// Buffer: 수신받은 데이터의 주소
	// Size: 수신받은 데이터의 크기
	// 바이트 스트림을 가공한 JPackets 객체
	template <typename PacketType>
	static TSharedPtr<JPackets> HandlePacket(uint16 TypeCode, uint8* Buffer, int32 Size)
	{
		PacketType Packet;
		if (Packet.ParseFromArray(Buffer + sizeof(FJPacketHeader), Size - sizeof(FJPacketHeader)) == false)
		{
			return nullptr;
		}

		TSharedPtr<JPackets> PacketPtr = MakeShared<JPackets>();
		PacketPtr->Type = TypeCode;
		PacketPtr->Packet.Set<PacketType>(Packet);

		return PacketPtr;
	}

public:
	static FPacket_INVALID Packet_INVALID_Delegate;
	static FPacket_S_VERIFY_TOKEN Packet_S_VERIFY_TOKEN_Delegate;
	static FPacket_S_LOBBY_CHAT Packet_S_LOBBY_CHAT_Delegate;
	static FPacket_S_LOBBY_REFRESH_ROOM Packet_S_LOBBY_REFRESH_ROOM_Delegate;
	static FPacket_S_LOBBY_CREATE_ROOM Packet_S_LOBBY_CREATE_ROOM_Delegate;
	static FPacket_S_LOBBY_ENTER_ROOM Packet_S_LOBBY_ENTER_ROOM_Delegate;
	static FPacket_S_ROOM_INFO Packet_S_ROOM_INFO_Delegate;
	static FPacket_S_ROOM_LEAVE Packet_S_ROOM_LEAVE_Delegate;
	static FPacket_S_ROOM_OTHER_ENTER Packet_S_ROOM_OTHER_ENTER_Delegate;
	static FPacket_S_ROOM_OTHER_LEAVE Packet_S_ROOM_OTHER_LEAVE_Delegate;
	static FPacket_S_ROOM_READY Packet_S_ROOM_READY_Delegate;
	static FPacket_S_ROOM_CHAT Packet_S_ROOM_CHAT_Delegate;
	static FPacket_S_ROOM_STANDBY_MATCH Packet_S_ROOM_STANDBY_MATCH_Delegate;
	static FPacket_S_ROOM_START_MATCH Packet_S_ROOM_START_MATCH_Delegate;
	static FPacket_S_MATCH_ALL_READY_TO_RECIEVE Packet_S_MATCH_ALL_READY_TO_RECIEVE_Delegate;
	static FPacket_S_MATCH_ITEM_GENERATED Packet_S_MATCH_ITEM_GENERATED_Delegate;
	static FPacket_S_MATCH_START Packet_S_MATCH_START_Delegate;
	static FPacket_S_MATCH_INFO Packet_S_MATCH_INFO_Delegate;
	static FPacket_S_MATCH_END Packet_S_MATCH_END_Delegate;
	static FPacket_S_MATCH_ITEM_SOMEONE_PICKUP Packet_S_MATCH_ITEM_SOMEONE_PICKUP_Delegate;
	static FPacket_S_MATCH_ITEM_SOMEONE_MOVE Packet_S_MATCH_ITEM_SOMEONE_MOVE_Delegate;
	static FPacket_S_MATCH_ITEM_SOMEONE_DROP Packet_S_MATCH_ITEM_SOMEONE_DROP_Delegate;
	static FPacket_S_MATCH_SCALE_ON_CHANGED Packet_S_MATCH_SCALE_ON_CHANGED_Delegate;
	static FPacket_S_MATCH_CHASER_ATTACK Packet_S_MATCH_CHASER_ATTACK_Delegate;
	static FPacket_S_MATCH_CHASER_HIT Packet_S_MATCH_CHASER_HIT_Delegate;
};