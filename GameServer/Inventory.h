#pragma once
#include "Item.h"

enum class InventoryErrorCode : UINT8
{
	SUCCESS = 0,
	ALREADY_HAVE = 10,
	EXCEEDING_WEIGHT_LIMITS = 11,
	NO_EMPTY_SPACE = 12,
	FROM_DOES_NOT_HAVE = 20,
	TO_EXCEEDING_WEIGHT_LIMITS = 21,
	TO_NO_EMPTY_SPACE = 22,
	DO_NOT_HAVE = 30,
};

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
	USE_MULTIPLE_LOCK;
	shared_ptr<Inventory> GetInventory() { return shared_from_this(); }
	int GetCurrentWeight() const { return currentWeight_; }
	int GetMaxWeight() const { return maxWeight_; }
	int GetIndex() const { return index_; }
	int GetItemsCount() const { return owningItems_.size(); }

	InventoryErrorCode TryAddItem(const shared_ptr<Item>& item);
	InventoryErrorCode RelocateItem(const shared_ptr<Inventory>& to, const shared_ptr<Item>& item, int slotIndex, bool isRotated);
	InventoryErrorCode DropItem(const shared_ptr<Item>& item, Vector position, Rotator rotation);
	static const wchar_t* GetErrorWhat(InventoryErrorCode errorCode);

protected:
	Point IndexToPoint(int index) { return {index % column_, index / column_}; }
	int PointToIndex(Point tile) { return tile.x + tile.y * column_; }
	bool CheckWeightLimit(int weight) { return weight + currentWeight_ <= maxWeight_; }
	bool CheckValidSlot(const shared_ptr<Item>& item, int slotIndex, bool isRotated);
	bool CheckValidPoint(int column, int row);
	void AddItemAt(const shared_ptr<Item>& item, int slotIndex);
	void PickUpItem(shared_ptr<Item> item);
	void AcquireItem(const shared_ptr<Item>& item);
	void ReleaseItem(const shared_ptr<Item>& item);
	void PrintInventory();

protected:
	USE_LOCK;
	int index_;
	int row_;
	int column_;
	int currentWeight_;
	int maxWeight_;
	weak_ptr<Player> ownerPlayer;
	vector<int> inventorySlots_;
	map<int, shared_ptr<Item>> owningItems_;
};
