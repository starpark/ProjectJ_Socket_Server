#include "pch.h"
#include "DataManager.h"

string DataManager::itemDataVersion_;
string DataManager::itemDataUpdatedDate_;
vector<ItemData> DataManager::itemData_;
vector<ItemSpawnData> DataManager::itemSpawnData_;
vector<CharacterSpawnData> DataManager::characterSpawnData_;
random_device DataManager::rd_;
default_random_engine DataManager::dre_(rd_());

bool DataManager::Init()
{
	try
	{
		LoadItemData();
		LoadItemSpawnData();
		LoadCharacterSpawnData();

		GLogHelper->Print(LogCategory::LOG_SUCCESS,
		                  L"Item Data Version: %s / Date: %s / Total Items: %d items / Total Item Spawn Points: %d points\n",
		                  UTF8_TO_WCHAR(itemDataVersion_.c_str()), UTF8_TO_WCHAR(itemDataUpdatedDate_.c_str()), itemData_.size(),
		                  itemSpawnData_.size());

		return true;
	}
	catch (const std::exception& e)
	{
		GLogHelper->Print(LogCategory::LOG_ERROR,
		                  L"DataManager Init Failure: %s\n",
		                  UTF8_TO_WCHAR(e.what()));
	}
	catch (const wchar_t* e)
	{
		GLogHelper->Print(LogCategory::LOG_ERROR, L"DataManager Init Failure: %s\n", e);
	}
	catch (...)
	{
		GLogHelper->Print(LogCategory::LOG_ERROR, L"DataManager Init Failure: Unexpected Error\n");
	}
	return false;
}

void DataManager::LoadItemData()
{
	ifstream file("./Data/ItemData.json");
	if (file.is_open())
	{
		json data = json::parse(file);
		file.close();

		if (data["Version"].is_string())
		{
			itemDataVersion_ = data["Version"];
		}
		else
		{
			throw L"LoadItemData: Version is not string";
		}

		if (data["Date"].is_string())
		{
			itemDataUpdatedDate_ = data["Date"];
		}
		else
		{
			throw L"LoadItemData: Date is not string";
		}

		if (data["Items"].is_array())
		{
			for (auto item : data["Items"])
			{
				itemData_.push_back(ItemData{
					item["ItemID"], item["Size_Height"], item["Size_Width"],
					item["Weight_Max"],
					item["Weight_Min"]
				});
			}
		}
	}
	else
	{
		throw L"The file ItemData.json does not exist";
	}
}

void DataManager::LoadItemSpawnData()
{
	ifstream file("./Data/ItemSpawnData.json");
	if (file.is_open())
	{
		json data = json::parse(file);
		file.close();

		if (data["Area_1"].is_array())
		{
			for (auto area : data["Area_1"])
			{
				itemSpawnData_.push_back(ItemSpawnData{area["Name"], area["X"], area["Y"], area["Z"], area["Roll"], area["Pitch"], area["Yaw"]});
			}
		}
		else
		{
			throw L"LoadItemSpawnData: Area_1 is not array";
		}

		if (data["Area_2"].is_array())
		{
			for (auto area : data["Area_2"])
			{
				itemSpawnData_.push_back(ItemSpawnData{area["Name"], area["X"], area["Y"], area["Z"], area["Roll"], area["Pitch"], area["Yaw"]});
			}
		}
		else
		{
			throw L"LoadItemSpawnData: Area_2 is not array";
		}

		if (data["Area_3"].is_array())
		{
			for (auto area : data["Area_3"])
			{
				itemSpawnData_.push_back(ItemSpawnData{area["Name"], area["X"], area["Y"], area["Z"], area["Roll"], area["Pitch"], area["Yaw"]});
			}
		}
		else
		{
			throw L"LoadItemSpawnData: Area_3 is not array";
		}

		if (data["Area_4"].is_array())
		{
			for (auto area : data["Area_4"])
			{
				itemSpawnData_.push_back(ItemSpawnData{area["Name"], area["X"], area["Y"], area["Z"], area["Roll"], area["Pitch"], area["Yaw"]});
			}
		}
		else
		{
			throw L"LoadItemSpawnData: Area_4 is not array";
		}

		if (data["CenterArea"].is_array())
		{
			for (auto area : data["CenterArea"])
			{
				itemSpawnData_.push_back(ItemSpawnData{area["Name"], area["X"], area["Y"], area["Z"], area["Roll"], area["Pitch"], area["Yaw"]});
			}
		}
		else
		{
			throw L"LoadItemSpawnData: CenterArea is not array";
		}
	}
	else
	{
		throw L"The file ItemSpawnData.json does not exist";
	}
}

void DataManager::LoadCharacterSpawnData()
{
	ifstream file("./Data/CharacterSpawnData.json");
	if (file.is_open())
	{
		json data = json::parse(file);
		file.close();

		if (data["Chaser"].is_object())
		{
			json& character = data["Chaser"];
			characterSpawnData_.push_back(CharacterSpawnData{
				character["X"], character["Y"], character["Z"], character["Roll"], character["Pitch"], character["Yaw"]
			});
		}
		else
		{
			throw L"LoadCharacterSpawnData: Chaser is not object";
		}

		if (data["FugitiveFirst"].is_object())
		{
			json& character = data["FugitiveFirst"];
			characterSpawnData_.push_back(CharacterSpawnData{
				character["X"], character["Y"], character["Z"], character["Roll"], character["Pitch"], character["Yaw"]
			});
		}
		else
		{
			throw L"LoadCharacterSpawnData: Chaser is not object";
		}

		if (data["FugitiveSecond"].is_object())
		{
			json& character = data["FugitiveSecond"];
			characterSpawnData_.push_back(CharacterSpawnData{
				character["X"], character["Y"], character["Z"], character["Roll"], character["Pitch"], character["Yaw"]
			});
		}
		else
		{
			throw L"LoadCharacterSpawnData: Chaser is not object";
		}

		if (data["FugitiveThird"].is_object())
		{
			json& character = data["FugitiveThird"];
			characterSpawnData_.push_back(CharacterSpawnData{
				character["X"], character["Y"], character["Z"], character["Roll"], character["Pitch"], character["Yaw"]
			});
		}
		else
		{
			throw L"LoadCharacterSpawnData: Chaser is not object";
		}
	}
	else
	{
		throw L"The file CharacterSpawnData.json does not exist";
	}
}

shared_ptr<Item> DataManager::GenerateItem(int index, Vector position, Rotator rotation)
{
	uniform_int_distribution<> idDist(0, itemData_.size() - 1);

	int id = idDist(dre_);

	uniform_int_distribution<> weightDist(itemData_[id].weightMin_, itemData_[id].weightMax_);

	int weight = (weightDist(dre_) / 10) * 10;

	auto newItem = make_shared<Item>(id, index, weight, Point{itemData_[id].sizeWidth_, itemData_[id].sizeHeight_}, position, rotation);

	return newItem;
}
