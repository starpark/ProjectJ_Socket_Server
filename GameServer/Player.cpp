#include "pch.h"
#include "Player.h"
#include "Match.h"
#include "GameSession.h"


Player::Player(int index, int row, int column, int maxWeight, int sessionID, string nickname)
	: Inventory(index, row, column, maxWeight), sessionID_(sessionID), nickname_(nickname)
{
}

Player::~Player()
{
	GLogHelper->Print(LogCategory::LOG_INFO, L"~Player()\n");
}


void Player::SetInfo(ProjectJ::Vector position, ProjectJ::Rotator rotation)
{
	worldPosition_.x_ = position.x();
	worldPosition_.y_ = position.y();
	worldPosition_.z_ = position.z();

	worldRotation_.pitch_ = rotation.pitch();
	worldRotation_.roll_ = rotation.roll();
	worldRotation_.yaw_ = rotation.yaw();
}
