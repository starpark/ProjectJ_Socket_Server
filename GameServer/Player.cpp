#include "pch.h"
#include "Player.h"
#include "Match.h"
#include "GameSession.h"


Player::Player(int index, int row, int column, int maxWeight, int sessionID, string nickname)
	: Inventory(index, row, column, maxWeight), state_(ProjectJ::MatchPlayerState::NONE), score_(0), sessionID_(sessionID), nickname_(nickname),
	  moveSpeed_(PLAYER_DEFAULT_MOVE_SPEED)
{
}

Player::~Player()
{
	GLogHelper->Print(LogCategory::LOG_INFO, L"~Player()\n");
}

ProjectJ::PlayerInfo* Player::GetPlayerInfo()
{
	auto playerInfo = new ProjectJ::PlayerInfo();
	auto position = new ProjectJ::Vector();
	auto velocity = new ProjectJ::Vector();
	auto rotation = new ProjectJ::Rotator();
	auto player = new ProjectJ::Player();

	{
		READ_LOCK;
		position->set_x(worldPosition_.x_);
		position->set_y(worldPosition_.y_);
		position->set_z(worldPosition_.z_);

		rotation->set_pitch(worldRotation_.pitch_);
		rotation->set_roll(worldRotation_.roll_);
		rotation->set_yaw(worldRotation_.yaw_);

		velocity->set_x(velocity_.x_);
		velocity->set_y(velocity_.y_);
		velocity->set_z(velocity_.z_);
	}

	player->set_account_id(GetID());
	player->set_nickname(GetNickname());

	playerInfo->set_state(state_);
	playerInfo->set_player_index(index_);
	playerInfo->set_allocated_player(player);
	playerInfo->set_allocated_position(position);
	playerInfo->set_allocated_rotation(rotation);
	playerInfo->set_allocated_velocity(velocity);
	playerInfo->set_move_speed(moveSpeed_);

	return playerInfo;
}

ProjectJ::PlayerInitInfo* Player::GetPlayerInitInfo()
{
	auto playerInitInfo = new ProjectJ::PlayerInitInfo();
	auto position = new ProjectJ::Vector();
	auto rotation = new ProjectJ::Rotator();
	auto player = new ProjectJ::Player();

	{
		READ_LOCK;
		position->set_x(worldPosition_.x_);
		position->set_y(worldPosition_.y_);
		position->set_z(worldPosition_.z_);

		rotation->set_pitch(worldRotation_.pitch_);
		rotation->set_roll(worldRotation_.roll_);
		rotation->set_yaw(worldRotation_.yaw_);
	}

	player->set_account_id(GetID());
	player->set_nickname(GetNickname());

	playerInitInfo->set_state(state_);
	playerInitInfo->set_player_index(index_);
	playerInitInfo->set_allocated_player(player);
	playerInitInfo->set_allocated_position(position);
	playerInitInfo->set_allocated_rotation(rotation);
	playerInitInfo->set_inv_size_row(GetRow());
	playerInitInfo->set_inv_size_colunm(GetColumn());
	playerInitInfo->set_inv_max_weight(GetMaxWeight());
	playerInitInfo->set_move_speed(moveSpeed_);

	return playerInitInfo;
}

InventoryErrorCode Player::TryAddItem(const shared_ptr<Item>& item)
{
	InventoryErrorCode result = Inventory::TryAddItem(item);

	SetMoveSpeed(GetCurrentMoveSpeed());

	return result;
}

InventoryErrorCode Player::RelocateItem(const shared_ptr<Inventory>& to, const shared_ptr<Item>& item, int slotIndex, bool isRotated)
{
	InventoryErrorCode result = Inventory::RelocateItem(to, item, slotIndex, isRotated);

	SetMoveSpeed(GetCurrentMoveSpeed());

	return result;
}

InventoryErrorCode Player::DropItem(const shared_ptr<Item>& item, Vector position, Rotator rotation)
{
	InventoryErrorCode result = Inventory::DropItem(item, position, rotation);

	SetMoveSpeed(GetCurrentMoveSpeed());

	return result;
}
