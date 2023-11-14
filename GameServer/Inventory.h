#pragma once
#include "Item.h"

class Inventory : public enable_shared_from_this<Inventory>
{
	enum
	{
		EMPTY_ITEM_ID = 0,
	};

public:
	Inventory(int index, int row, int column, int maxWeight);
	virtual ~Inventory();

public:
	shared_ptr<Inventory> GetInventory() { return shared_from_this(); }
	int GetCurrentWeight() const { return currentWeight_; }
	int GetMaxWeight() const { return maxWeight_; }
	int GetIndex() const { return index_; }
	void PrintTest();

	bool TryAddItem(const shared_ptr<Item>& item);
	bool RelocateItem(const shared_ptr<Inventory>& to, const shared_ptr<Item>& item, int slotIndex, bool isRotated);
	bool DropItem(const shared_ptr<Item>& item, ProjectJ::Vector vector, ProjectJ::Rotator rotate);

protected:
	Point IndexToPoint(int index) { return {index % column_, index / column_}; }
	int PointToIndex(Point tile) { return tile.x + tile.y * column_; }
	bool CheckWeightLimit(int weight) { return weight + currentWeight_ <= maxWeight_; }
	bool CheckValidSlot(const shared_ptr<Item>& item, int slotIndex, bool isRotated);
	bool CheckValidPoint(int column, int row);
	void RelocateItemAt(const shared_ptr<Item>& item, int slotIndex);
	void AddItemAt(const shared_ptr<Item>& item, int slotIndex);
	void PickUpItem(shared_ptr<Item> item);
	void AcquireItem(const shared_ptr<Item>& item);
	void ReleaseItem(const shared_ptr<Item>& item);

protected:
	int index_;
	int row_;
	int column_;
	int currentWeight_;
	int maxWeight_;
	weak_ptr<Player> ownerPlayer;
	vector<int> inventorySlots_;
	map<int, shared_ptr<Item>> owningItems_;
};
