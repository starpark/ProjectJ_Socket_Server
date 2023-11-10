#pragma once
#include "Item.h"

class Inventory
{
	enum
	{
		EMPTY_ITEM_ID = 0,
		MAX_COLUMN = 6,
		MAX_ROW = 15,
		MAX_WEIGHT = 50000
	};

public:
	Inventory();
	~Inventory();

public:
	bool TryAddItem(shared_ptr<Item> item, shared_ptr<Player> player);
	bool RelocateItem(shared_ptr<Item> item, int slotIndex, bool isRotated);
	bool DropItem(shared_ptr<Item> item, ProjectJ::Vector vector, ProjectJ::Rotator rotate);
	void PrintTest();

private:
	Point IndexToPoint(int index) { return {index % MAX_COLUMN, index / MAX_COLUMN}; }
	int PointToIndex(Point tile) { return tile.X + tile.Y * MAX_COLUMN; }
	bool CheckWeightLimit(int weight) { return weight + currentWeight_ <= MAX_WEIGHT; }
	bool CheckValidSlot(const shared_ptr<Item>& item, int slotIndex, bool isRotated);
	bool CheckValidPoint(int column, int row);
	void RelocateItemAt(const shared_ptr<Item>& item, int slotIndex);
	void AddItemAt(const shared_ptr<Item>& item, int slotIndex);
	void PickUpItem(shared_ptr<Item> item);
	void AcquireItem(const shared_ptr<Item>& item);
	void ReleaseItem(const shared_ptr<Item>& item);

private:
	weak_ptr<Player> ownerPlayer;
	int currentWeight_ = 0;
	vector<int> inventorySlots_;
	map<int, shared_ptr<Item>> owningItems_;
};
