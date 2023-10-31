#pragma once
#include "Struct.pb.h"

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
	mutex& GetLock() { return lock_; }
	int GetItemID() const { return itemID_; }
	int GetWeight() const { return weight_; }
	int GetTopLeftIndex() const { return topLeftIndex_; }
	bool GetIsRotated() const { return bIsRotated_; }
	void SetIsRotated(bool rotated) { bIsRotated_ = rotated; }
	ProjectJ::Vector GetVector() const { return vector_; }
	ProjectJ::Rotator GetRotation() const { return rotation_; }
	Point GetSize() const { return size_; }
	shared_ptr<Player> GetOwner() const { return owner_; }

public:
	void DroppedToWorld(ProjectJ::Vector vector, ProjectJ::Rotator rotation);
	void PickedUpByPlayer(shared_ptr<Player> player, int topLeftIndex, bool rotated);
	void RelocatedInInventory(int topLeftIndex, bool rotated);

private:
	mutex lock_;
	int itemID_;
	int weight_;
	Point size_;

	ProjectJ::Vector vector_;
	ProjectJ::Rotator rotation_;

	int topLeftIndex_ = -1;
	bool bIsRotated_ = false;
	shared_ptr<Player> owner_ = nullptr; // TODO
};
