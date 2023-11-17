#include "pch.h"
#include "Inventory.h"

Inventory::Inventory(int index, int row, int column, int maxWeight)
	: index_(index), row_(row), column_(column), currentWeight_(0), maxWeight_(maxWeight)
{
	inventorySlots_.assign(column * row, EMPTY_ITEM_ID);
}

Inventory::~Inventory()
{
	owningItems_.clear();
}

InventoryErrorCode Inventory::TryAddItem(const shared_ptr<Item>& item)
{
	WRITE_LOCK;
	if (owningItems_.find(item->index) != owningItems_.end())
	{
		return InventoryErrorCode::ALREADY_HAVE;
	}
	UINT32 flag = item->ownerFlag.load();

	if (CheckWeightLimit(item->weight) == false)
	{
		return InventoryErrorCode::EXCEEDING_WEIGHT_LIMITS;
	}

	for (int slotIndex = 0; slotIndex < row_ * column_; slotIndex++)
	{
		if (CheckValidSlot(item, slotIndex, false))
		{
			item->topLeftIndex = slotIndex;
			item->bIsRotated = false;

			AddItemAt(item, slotIndex);

			return InventoryErrorCode::SUCCESS;
		}

		if (CheckValidSlot(item, slotIndex, true))
		{
			item->topLeftIndex = slotIndex;
			item->bIsRotated = true;

			AddItemAt(item, slotIndex);

			return InventoryErrorCode::SUCCESS;
		}
	}

	return InventoryErrorCode::ALREADY_HAVE;
}

InventoryErrorCode Inventory::RelocateItem(const shared_ptr<Inventory>& to, const shared_ptr<Item>& item, int slotIndex, bool isRotated)
{
	WRITE_LOCK;
	if (owningItems_.find(item->index) == owningItems_.end())
	{
		return InventoryErrorCode::FROM_DOES_NOT_HAVE;
	}

	PickUpItem(item);

	if (to->CheckValidSlot(item, slotIndex, isRotated))
	{
		if(to->CheckWeightLimit(item->weight))
		{
			item->topLeftIndex = slotIndex;
			item->bIsRotated = isRotated;

			to->AddItemAt(item, slotIndex);

			return InventoryErrorCode::SUCCESS;
		}
		else
		{
			return InventoryErrorCode::TO_EXCEEDING_WEIGHT_LIMITS;
		}
	}

	AddItemAt(item, item->topLeftIndex);

	throw InventoryErrorCode::TO_NO_EMPTY_SPACE;
}

InventoryErrorCode Inventory::DropItem(const shared_ptr<Item>& item, ProjectJ::Vector vector, ProjectJ::Rotator rotate)
{
	WRITE_LOCK;
	if (owningItems_.find(item->index) == owningItems_.end())
	{
		return InventoryErrorCode::DO_NOT_HAVE;
	}

	PickUpItem(item);

	return InventoryErrorCode::SUCCESS;
}

bool Inventory::CheckValidSlot(const shared_ptr<Item>& item, int slotIndex, bool isRotated)
{
	Point tile = IndexToPoint(slotIndex);
	Point size = item->size;
	if (isRotated)
	{
		swap(size.x, size.y);
	}

	for (int rowIndex = 0; rowIndex < size.y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.x; columnIndex++)
		{
			if (CheckValidPoint(tile.x + columnIndex, tile.y + rowIndex) == false)
			{
				return false;
			}
		}
	}

	return true;
}

bool Inventory::CheckValidPoint(int column, int row)
{
	if (column < 0 || row < 0 || column >= column_ || row >= row_)
	{
		return false;
	}

	int index = PointToIndex({column, row});
	if (inventorySlots_[index] != EMPTY_ITEM_ID)
	{
		return false;
	}

	return true;
}

void Inventory::RelocateItemAt(const shared_ptr<Item>& item, int slotIndex)
{
}

void Inventory::AddItemAt(const shared_ptr<Item>& item, int slotIndex)
{
	Point tile = IndexToPoint(slotIndex);
	Point size = item->size;

	if (item->bIsRotated)
	{
		swap(size.x, size.y);
	}

	for (int rowIndex = 0; rowIndex < size.y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.x; columnIndex++)
		{
			inventorySlots_[PointToIndex({tile.x + columnIndex, tile.y + rowIndex})] = item->id;
		}
	}

	AcquireItem(item);
}

void Inventory::PickUpItem(shared_ptr<Item> item)
{
	Point tile = IndexToPoint(item->topLeftIndex);
	Point size = item->size;

	if (item->bIsRotated)
	{
		swap(size.x, size.y);
	}

	for (int rowIndex = 0; rowIndex < size.y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.x; columnIndex++)
		{
			int slotIndex = PointToIndex({tile.x + columnIndex, tile.y + rowIndex});
			if (inventorySlots_[slotIndex] != item->id)
			{
				// TODO ERROR
			}

			inventorySlots_[slotIndex] = EMPTY_ITEM_ID;
		}
	}

	ReleaseItem(item);
}

void Inventory::AcquireItem(const shared_ptr<Item>& item)
{
	owningItems_.insert({item->index, item});
	currentWeight_ += item->weight;
}

void Inventory::ReleaseItem(const shared_ptr<Item>& item)
{
	owningItems_.erase(item->index);
	currentWeight_ -= item->weight;
}

const wchar_t* Inventory::GetErrorWhat(InventoryErrorCode errorCode)
{
	switch (errorCode)
	{
	case InventoryErrorCode::SUCCESS:
		return L"No Error";
	case InventoryErrorCode::ALREADY_HAVE:
		return L"Already Have The Item";
	case InventoryErrorCode::EXCEEDING_WEIGHT_LIMITS:
		return L"Exceeding Inventory Weight Limits";
	case InventoryErrorCode::NO_EMPTY_SPACE:
		return L"There Are No Empty Spaces In The Inventory";
	case InventoryErrorCode::FROM_DOES_NOT_HAVE:
		return L"From's Inventory Doesn't Have ITem";
	case InventoryErrorCode::TO_EXCEEDING_WEIGHT_LIMITS:
		return L"Exceeding To's Inventory Weight Limits";
	case InventoryErrorCode::TO_NO_EMPTY_SPACE:
		return L"There Are No Empty Spaces In To's Inventory";
	case InventoryErrorCode::DO_NOT_HAVE:
		return L"Don't Have The Item";
	default:
		return L"Unexpected Error";
	}
}
