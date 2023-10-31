#include "pch.h"
#include "Item.h"

Item::Item(int id, int weight, Point size, ProjectJ::Vector vector, ProjectJ::Rotator rotate)
	: itemID_(id), weight_(weight), size_(size), vector_(vector), rotation_(rotate)
{
}

Item::~Item()
{
	owner_ = nullptr;
}

void Item::DroppedToWorld(ProjectJ::Vector vector, ProjectJ::Rotator rotation)
{
	owner_ = nullptr;
	vector_ = vector;
	rotation_ = rotation;
	topLeftIndex_ = -1;
}

void Item::PickedUpByPlayer(shared_ptr<Player> player, int topLeftIndex, bool rotated)
{
	owner_ = player;
	topLeftIndex_ = topLeftIndex;
	bIsRotated_ = rotated;
}

void Item::RelocatedInInventory(int topLeftIndex, bool rotated)
{
	topLeftIndex_ = topLeftIndex;
	bIsRotated_ = rotated;
}
