#pragma once
#include "Struct.pb.h"
#include "nlohmann/json.hpp"

#define INVALID_ITEM_OWNER_ID -1
#define INVALID_TOP_LEFT_INDEX -1
#define INVALID_OWNER_ID -1


using json = nlohmann::json;

class Player;

struct Item
{
	enum : UINT32
	{
		EMPTY_OWNER_ID = 0x0000'0000,
		OWNERSHIP_ID_MASK = 0xFFFF'0000,
		OWNER_ID_MASK = 0x0000'FFFF
	};

public:
	Item(int id, int index, int weight, Point size, Vector vector, Rotator rotate)
		: id_(id), index_(index), weight_(weight), size_(size), vector_(vector), rotation_(rotate)
	{
		ownerID_.store(EMPTY_OWNER_ID);
	}

	~Item();

	const int id_;
	const int index_;
	const int weight_;
	const Point size_;

	Vector vector_;
	Rotator rotation_;

	int topLeftIndex_ = INVALID_TOP_LEFT_INDEX;
	bool bIsRotated_ = false;
	// 0: chaser, 1~3: fugitive, 4-7: scale
	// 상위 16 비트: 현재 조작중인 player index
	// 하위 16 비트: 현재 소유중인 player index
	atomic<UINT32> ownerID_; // TODO
};
