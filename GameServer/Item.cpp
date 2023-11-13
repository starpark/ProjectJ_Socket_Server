#include "pch.h"
#include "Item.h"

string ItemManager::version_;
string ItemManager::updatedDate_;
vector<ItemData> ItemManager::itemData_;

bool ItemManager::Init()
{
	ifstream file("./Data/ItemData.json");
	if (file.is_open())
	{
		try
		{
			json data = json::parse(file);
			file.close();

			if (data["Version"].is_string())
			{
				version_ = data["Version"];
			}
			else
			{
				throw L"Version is not string";
			}

			if (data["Date"].is_string())
			{
				updatedDate_ = data["Date"];
			}
			else
			{
				throw L"Date is not string";
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

			GLogHelper->Print(LogCategory::Log_SUCCESS,
			                  L"ItemManager Version: %s Date: %s Total: %d items\n",
			                  UTF8_TO_WCHAR(version_.c_str()),UTF8_TO_WCHAR(updatedDate_.c_str()), itemData_.size());

			return true;
		}
		catch (const std::exception& e)
		{
			GLogHelper->Print(LogCategory::Log_ERROR,
			                  L"ItemManager Init Failure: %s\n",
			                  UTF8_TO_WCHAR(e.what()));
			return false;
		}
		catch (const wchar_t* e)
		{
			GLogHelper->Print(LogCategory::Log_ERROR, L"ItemManager Init Failure: %s\n", e);
			return false;
		}
	}
	GLogHelper->Print(LogCategory::Log_ERROR, L"ItemManager Init Failure: File Open\n");
	return false;
}

shared_ptr<Item> ItemManager::GenerateItem()
{
	return nullptr;
}
