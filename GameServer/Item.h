#pragma once
#include "Struct.pb.h"
#include "nlohmann/json.hpp"

#define INVALID_ITEM_OWNER_ID -1
#define INVALID_TOP_LEFT_INDEX -1

using json = nlohmann::json;

class Player;

struct Item
{
	enum : UINT32
	{
		HANDLE_PLAYER_INDEX = 16,
		EMPTY_OWNER_ID = 0x0000'0000,
		HANDLE_MASK = 0xF000'0000,
		HANDLE_PLAYER_MASK = 0x0FFF'0000,
		OWNED_MASK = 0x0000'F000,
		OWNER_PLAYER_MASK = 0x0000'0FFF
	};

public:
	Item(int inId, int inIndex, int inWeight, int inRow, int inColunm, Vector inVector, Rotator inRotate)
		: id(inId), index(inIndex), weight(inWeight), row(inRow), column(inColunm), position(inVector), rotation(inRotate)
	{
		ownerFlag.store(EMPTY_OWNER_ID);
	}

	~Item()
	{
		ownedPlayerIndexSet.clear();
		prevOwnedPlayerIndex = nullptr;
	}

	bool CheckFirstAdd(const shared_ptr<Player>& playerIndex)
	{
		if (ownedPlayerIndexSet.find(playerIndex) == ownedPlayerIndexSet.end())
		{
			ownedPlayerIndexSet.insert(playerIndex);
			return true;
		}
		return false;
	}

	const int id;
	const int index;
	const int weight;
	const int row;
	const int column;

	Vector position;
	Rotator rotation;

	// index 는 chaser - 0, fugitive - 1 ~ 3, scale 4 ~ 7 로 배정 
	// [0000][0000 0000 0000] [0000][0000 0000 0000]
	// 1 ~ 4	4 비트: 조작 여부 FFFF: 누군가 조작 중/ 0000: 아무도 조작중이 아님
	// 5 ~ 16	12 비트: 현재 조작중인 player index
	// 17 ~ 20	4 비트: 소유 여부 FFFF: 누군가 소우 중 / 0000: 아무도 소유중이 아님
	// 21 ~ 32	12 비트: 현재 소유중인 player index
	atomic<UINT32> ownerFlag; // TODO
	int topLeftIndex = INVALID_TOP_LEFT_INDEX;
	bool bIsRotated = false;
	set<shared_ptr<Player>> ownedPlayerIndexSet;
	shared_ptr<Player> prevOwnedPlayerIndex;
};
