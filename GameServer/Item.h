#pragma once
#include "Struct.pb.h"
#include <fstream>
#include "nlohmann/json.hpp"

#define INVALID_ITEM_OWNER_ID -1
#define INVALID_TOP_LEFT_INDEX -1

using json = nlohmann::json;

class Player;

struct Point
{
	int X;
	int Y;
};


class Item
{
public:
	Item(int id, int weight, Point size, ProjectJ::Vector vector, ProjectJ::Rotator rotate);
	~Item();

public:
	int GetItemID() const { return itemID_; }
	int GetWeight() const { return weight_; }
	int GetTopLeftIndex() const { return topLeftIndex_; }
	bool GetIsRotated() const { return bIsRotated_; }
	void SetIsRotated(bool rotated) { bIsRotated_ = rotated; }
	ProjectJ::Vector GetVector() const { return vector_; }
	ProjectJ::Rotator GetRotation() const { return rotation_; }
	Point GetSize() const { return size_; }
	int GetOwnerID() const { return ownerID_; }

public:
	void DroppedToWorld(ProjectJ::Vector vector, ProjectJ::Rotator rotation);
	void PickedUpByPlayer(shared_ptr<Player> player, int topLeftIndex, bool rotated);
	void RelocatedInInventory(int topLeftIndex, bool rotated);

private:
	int itemID_;
	int weight_;
	Point size_;

	ProjectJ::Vector vector_;
	ProjectJ::Rotator rotation_;

	int topLeftIndex_ = INVALID_TOP_LEFT_INDEX;
	bool bIsRotated_ = false;
	// 0: chaser, 1~3: fugitive, 4-7: scale
	int ownerID_ = INVALID_ITEM_OWNER_ID;
};

struct ItemData
{
	ItemData(json id, json category, json height, json width, json max, json min)
	{
		if (id.is_string() == false) throw "ItemData ItemId is not string";
		if (category.is_string() == false) throw "ItemData Category is not string";
		if (height.is_number_integer() == false) throw "ItemData Size Height is not integer";
		if (width.is_number_integer() == false) throw "ItemData Size Width is not intger";
		if (max.is_number_integer() == false) throw "ItemData Weight Max is not intger";
		if (min.is_number_integer() == false) throw "ItemData Weight Min is not intger";

		id_ = id;
		category_ = category_;
		sizeHeight_ = height;
		sizeWidth_ = width;
		weightMax_ = max;
		weightMin_ = min;
	}

	string id_;
	string category_;
	int sizeHeight_;
	int sizeWidth_;
	int weightMax_;
	int weightMin_;
};

class ItemManager
{
public:
	static bool Init();
	static int GetItemsSize() { return itemData_.size(); }
	static shared_ptr<Item> GenerateItem();

private:
	static string version_;
	static string updatedDate_;
	static vector<ItemData> itemData_;
};
