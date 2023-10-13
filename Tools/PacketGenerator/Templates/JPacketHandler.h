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
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
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

{%- for pkt in parser.send_pkt %}
bool Handle_{{pkt.name}}(UWorld* World, ProjectJ::{{pkt.name}}& Packet, float DeltaSeconds);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FPacket_{{pkt.name}}, UWorld*, ProjectJ::{{pkt.name}}&, float);
{%- endfor %}

// 소켓 수신 데이터 처리 및 송신 버퍼 생성 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: {{date}} 자동 생성
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
{%- for pkt in parser.send_pkt %}
		GPacketProcessor[PKT_{{pkt.name}}] = [](UWorld* World, const TSharedPtr<JPackets>& PacketPtr, float DeltaSeconds) {return ProcessPacket<ProjectJ::{{pkt.name}}>(Handle_{{pkt.name}}, World, PacketPtr, DeltaSeconds);};
{%- endfor %}

		// Handler 바인딩
{%- for pkt in parser.send_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](uint16 TypeCode, uint8* Buffer, int32 Size) {return HandlePacket<ProjectJ::{{pkt.name}}>(TypeCode, Buffer, Size);};
{%- endfor %}
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
{%- for pkt in parser.recv_pkt %}
	// {{pkt.name}}를 직렬화한 FJSendBuffer 생성 함수
	// Packet: {{pkt.name}} 객체
	// 생성된 FJSendBuffer 스마트포인터 반환
	static TSharedPtr<FJSendBuffer> MakeSendBuffer(ProjectJ::{{pkt.name}}& Packet) {return MakeSendBuffer(Packet, PKT_{{pkt.name}});}
{%- endfor %}

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
{%- for pkt in parser.send_pkt %}
	static FPacket_{{pkt.name}} Packet_{{pkt.name}}_Delegate;
{%- endfor %}
};
