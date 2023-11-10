#include "pch.h"
#include "Inventory.h"

Inventory::Inventory()
{
	inventorySlots_.assign(MAX_COLUMN * MAX_ROW, EMPTY_ITEM_ID);
}

Inventory::~Inventory()
{
	owningItems_.clear();
}

bool Inventory::TryAddItem(shared_ptr<Item> item, shared_ptr<Player> player)
{
	if (owningItems_.find(item->GetItemID()) != owningItems_.end() || item->GetOwnerID() != INVALID_ITEM_OWNER_ID)
	{
		return false;
	}

	if (CheckWeightLimit(item->GetWeight()) == false)
	{
		return false;
	}

	for (int slotIndex = 0; slotIndex < MAX_ROW * MAX_COLUMN; slotIndex++)
	{
		if (CheckValidSlot(item, slotIndex, false))
		{
			item->PickedUpByPlayer(player, slotIndex, false);
			AddItemAt(item, slotIndex);
			AcquireItem(item);

			return true;
		}

		if (CheckValidSlot(item, slotIndex, true))
		{
			item->PickedUpByPlayer(player, slotIndex, true);
			AddItemAt(item, slotIndex);
			AcquireItem(item);

			return true;
		}
	}

	return false;
}

bool Inventory::RelocateItem(shared_ptr<Item> item, int slotIndex, bool isRotated)
{
	if (owningItems_.find(item->GetItemID()) == owningItems_.end() /* TODO Onwer Check*/)
	{
		return false;
	}

	PickUpItem(item);

	if (CheckValidSlot(item, slotIndex, isRotated))
	{
		item->RelocatedInInventory(slotIndex, isRotated);
		AddItemAt(item, slotIndex);

		return true;
	}

	AddItemAt(item, item->GetTopLeftIndex());

	return false;
}

bool Inventory::DropItem(shared_ptr<Item> item, ProjectJ::Vector vector, ProjectJ::Rotator rotate)
{
	if (owningItems_.find(item->GetItemID()) == owningItems_.end())
	{
		return false;
	}

	PickUpItem(item);
	ReleaseItem(item);
	item->DroppedToWorld(vector, rotate);

	return true;
}

void Inventory::PrintTest()
{
	for (int rowIndex = 0; rowIndex < MAX_ROW; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < MAX_COLUMN; columnIndex++)
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

	cout << "현재 무게/최대 무게: " << currentWeight_ << "/" << MAX_WEIGHT << endl;
}


bool Inventory::CheckValidSlot(const shared_ptr<Item>& item, int slotIndex, bool isRotated)
{
	Point tile = IndexToPoint(slotIndex);
	Point size = item->GetSize();
	if (isRotated)
	{
		swap(size.X, size.Y);
	}

	for (int rowIndex = 0; rowIndex < size.Y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.X; columnIndex++)
		{
			if (CheckValidPoint(tile.X + columnIndex, tile.Y + rowIndex) == false)
			{
				return false;
			}
		}
	}

	return true;
}

bool Inventory::CheckValidPoint(int column, int row)
{
	if (column < 0 || row < 0 || column >= MAX_COLUMN || row >= MAX_ROW)
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
	Point size = item->GetSize();

	if (item->GetIsRotated())
	{
		swap(size.X, size.Y);
	}

	for (int rowIndex = 0; rowIndex < size.Y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.X; columnIndex++)
		{
			inventorySlots_[PointToIndex({tile.X + columnIndex, tile.Y + rowIndex})] = item->GetItemID();
		}
	}
}

void Inventory::PickUpItem(shared_ptr<Item> item)
{
	Point tile = IndexToPoint(item->GetTopLeftIndex());
	Point size = item->GetSize();

	if (item->GetIsRotated())
	{
		swap(size.X, size.Y);
	}

	for (int rowIndex = 0; rowIndex < size.Y; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < size.X; columnIndex++)
		{
			int slotIndex = PointToIndex({tile.X + columnIndex, tile.Y + rowIndex});
			if (inventorySlots_[slotIndex] != item->GetItemID())
			{
				// TODO ERROR
			}

			inventorySlots_[slotIndex] = EMPTY_ITEM_ID;
		}
	}
}

void Inventory::AcquireItem(const shared_ptr<Item>& item)
{
	owningItems_.insert({item->GetItemID(), item});
	currentWeight_ += item->GetWeight();
}

void Inventory::ReleaseItem(const shared_ptr<Item>& item)
{
	owningItems_.erase(item->GetItemID());
	currentWeight_ -= item->GetWeight();
}
