#include "pch.h"
#include "Player.h"
#include "Match.h"
#include "GameSession.h"


Player::Player(int index, int row, int column, int maxWeight, const shared_ptr<Match>& match)
	: Inventory(index, row, column, maxWeight), match_(match)
{
}

Player::~Player()
{
	GLogHelper->Print(LogCategory::Log_INFO, L"~Player()\n");
	match_ = nullptr;
}


void Player::SetInfo(ProjectJ::Vector position, ProjectJ::Rotator rotation)
{
	worldPosition_.x = position.x();
	worldPosition_.y = position.y();
	worldPosition_.z = position.z();

	worldRotation_.pitch = rotation.pitch();
	worldRotation_.roll = rotation.roll();
	worldRotation_.yaw = rotation.yaw();
}
