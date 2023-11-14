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

bool Inventory::TryAddItem(const shared_ptr<Item>& item)
{
	WRITE_LOCK;
	if (owningItems_.find(item->index_) != owningItems_.end() || item->ownerID_.load() != Item::EMPTY_OWNER_ID)
	{
		return false;
	}

	if (CheckWeightLimit(item->weight_) == false)
	{
		return false;
	}

	for (int slotIndex = 0; slotIndex < row_ * column_; slotIndex++)
	{
		if (CheckValidSlot(item, slotIndex, false))
		{
			item->topLeftIndex_ = slotIndex;
			item->bIsRotated_ = false;

			AddItemAt(item, slotIndex);

			return true;
		}

		if (CheckValidSlot(item, slotIndex, true))
		{
			item->topLeftIndex_ = slotIndex;
			item->bIsRotated_ = true;

			AddItemAt(item, slotIndex);

			return true;
		}
	}

	return false;
}

bool Inventory::RelocateItem(const shared_ptr<Inventory>& to, const shared_ptr<Item>& item, int slotIndex, bool isRotated)
{
	WRITE_LOCK;
	if (owningItems_.find(item->index_) == owningItems_.end())
	{
		return false;
	}

	PickUpItem(item);

	if (to->CheckValidSlot(item, slotIndex, isRotated) && to->CheckWeightLimit(item->weight_))
	{
		item->topLeftIndex_ = slotIndex;
		item->bIsRotated_ = isRotated;

		to->AddItemAt(item, slotIndex);

		return true;
	}

	AddItemAt(item, item->topLeftIndex_);

	return false;
}

bool Inventory::DropItem(const shared_ptr<Item>& item, ProjectJ::Vector vector, ProjectJ::Rotator rotate)
{
	WRITE_LOCK;
	if (owningItems_.find(item->index_) == owningItems_.end())
	{
		return false;
	}

	PickUpItem(item);

	return true;
}

void Inventory::PrintTest()
{
	for (int rowIndex = 0; rowIndex < row_; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < column_; columnIndex++)
		{
			int slotIndex = PointToIndex({columnIndex, rowIndex});
			cout << inventorySlots_[slotIndex] << " ";
		}
		cout << endl;
	}
	cout << endl;

	cout << "소유중인 아이템: ";
	for (auto items : owningItems_)
	{
		cout << items.first << " ";
	}
	cout << endl;

	cout << "현재 무게/최대 무게: " << currentWeight_ << "/" << maxWeight_ << endl;
}


bool Inventory::CheckValidSlot(const shared_ptr<Item>& item, int slotIndex, bool isRotated)
{
	Point tile = IndexToPoint(slotIndex);
	Point size = item->size_;
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
	Point size = item->size_;

	if (item->bIsRotated_)
	{
		swap(size.x, size.y);
	}

	for (int rowIndex = 0; rowIndex < size.y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.x; columnIndex++)
		{
			inventorySlots_[PointToIndex({tile.x + columnIndex, tile.y + rowIndex})] = item->id_;
		}
	}

	AcquireItem(item);
}

void Inventory::PickUpItem(shared_ptr<Item> item)
{
	Point tile = IndexToPoint(item->topLeftIndex_);
	Point size = item->size_;

	if (item->bIsRotated_)
	{
		swap(size.x, size.y);
	}

	for (int rowIndex = 0; rowIndex < size.y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.x; columnIndex++)
		{
			int slotIndex = PointToIndex({tile.x + columnIndex, tile.y + rowIndex});
			if (inventorySlots_[slotIndex] != item->id_)
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
	owningItems_.insert({item->index_, item});
	currentWeight_ += item->weight_;
}

void Inventory::ReleaseItem(const shared_ptr<Item>& item)
{
	owningItems_.erase(item->index_);
	currentWeight_ -= item->weight_;
}
