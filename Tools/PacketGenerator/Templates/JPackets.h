// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Network/Protocol/Message.pb.h"

using JPacketTypes = TVariant<
{%- for pkt in parser.send_pkt %}
{%- if loop.index < parser.send_pkt | length %}
	ProjectJ::{{pkt.name}},
{%- else %}
	ProjectJ::{{pkt.name}}
{%- endif %}
{%- endfor %}
>;

// 패킷 래핑 클래스
// 최초 작성자: 박별
// 수정자: 
// 최종 수정일: {{date}} 자동 생성
class PROJECTJ_API JPackets
{
public:
	uint16 Type = 0;
	JPacketTypes Packet;
};
