#pragma once
#include <random>
#include "Item.h"

struct ItemData
{
	ItemData(json id, json height, json width, json max, json min)
	{
		if (id.is_number_integer() == false) throw L"ItemData ItemID is not integer";
		if (height.is_number_integer() == false) throw L"ItemData Size Height is not integer";
		if (width.is_number_integer() == false) throw L"ItemData Size Width is not integer";
		if (max.is_number_integer() == false) throw L"ItemData Weight Max is not integer";
		if (min.is_number_integer() == false) throw L"ItemData Weight Min is not integer";

		id_ = id;
		sizeHeight_ = height;
		sizeWidth_ = width;
		weightMax_ = max;
		weightMin_ = min;
	}

	int id_;
	int sizeHeight_;
	int sizeWidth_;
	int weightMax_;
	int weightMin_;
};

struct ItemSpawnData
{
	ItemSpawnData(json name, json x, json y, json z, json roll, json pitch, json yaw)
	{
		if (name.is_string() == false) throw L"ItemSpawnData Name is not string";
		if (x.is_string() == false) throw L"ItemSpawnData X is not string";
		if (y.is_string() == false) throw L"ItemSpawnData Y is not string";
		if (z.is_string() == false) throw L"ItemSpawnData Z is not string";
		if (roll.is_string() == false) throw L"ItemSpawnData Roll is not string";
		if (pitch.is_string() == false) throw L"ItemSpawnData Pitch is not string";
		if (yaw.is_string() == false) throw L"ItemSpawnData Yaw is not string";

		name_ = name;
		position_ = Vector{stof(x.get<string>()), stof(z.get<string>()), stof(y.get<string>())};
		rotation_ = Rotator{stof(roll.get<string>()), stof(pitch.get<string>()), stof(yaw.get<string>())};
	}

	string name_;
	Vector position_;
	Rotator rotation_;
};

struct CharacterSpawnData
{
	CharacterSpawnData(json x, json y, json z, json roll, json pitch, json yaw)
	{
		if (x.is_string() == false) throw L"CharacterSpawnData X is not string";
		if (y.is_string() == false) throw L"CharacterSpawnData Y is not string";
		if (z.is_string() == false) throw L"CharacterSpawnData Z is not string";
		if (roll.is_string() == false) throw L"CharacterSpawnData Roll is not string";
		if (pitch.is_string() == false) throw L"CharacterSpawnData Pitch is not string";
		if (yaw.is_string() == false) throw L"CharacterSpawnData Yaw is not string";

		position_ = Vector{stof(x.get<string>()), stof(z.get<string>()), stof(y.get<string>())};
		rotation_ = Rotator{stof(roll.get<string>()), stof(pitch.get<string>()), stof(yaw.get<string>())};
	}

	Vector position_;
	Rotator rotation_;
};

class DataManager
{
public:
	static bool Init();

	static const vector<ItemSpawnData>& GetItemSpawnData() { return itemSpawnData_; }
	static shared_ptr<Item> GenerateItem(int index, Vector position, Rotator rotation);
	static default_random_engine& GetRandomEngine() { return dre_; }

private:
	static void LoadItemData();
	static void LoadItemSpawnData();
	static void LoadCharacterSpawnData();

private:
	static string itemDataVersion_;
	static string itemDataUpdatedDate_;
	static vector<ItemData> itemData_;
	static vector<ItemSpawnData> itemSpawnData_;
	static vector<CharacterSpawnData> characterSpawnData_;
	static random_device rd_;
	static default_random_engine dre_;
};
