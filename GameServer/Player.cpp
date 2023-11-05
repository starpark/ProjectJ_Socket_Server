#include "pch.h"
#include "Player.h"
#include "Match.h"
#include "GameSession.h"

Player::Player(const shared_ptr<Match>& match)
	: match_(match)
{
	inventory_ = make_shared<Inventory>();
}

Player::~Player()
{
	GLogHelper->Reserve(LogCategory::Log_INFO, "~Player()\n");
	match_ = nullptr;
	inventory_ = nullptr;
}

void Player::SetInfo(ProjectJ::Vector position, ProjectJ::Rotator rotation)
{
	WRITE_LOCK;
	worldPosition_.x = position.x();
	worldPosition_.y = position.y();
	worldPosition_.z = position.z();

	worldRotation_.pitch = rotation.pitch();
	worldRotation_.roll = rotation.roll();
	worldRotation_.yaw = rotation.yaw();
}
