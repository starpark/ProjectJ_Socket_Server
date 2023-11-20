#include "pch.h"
#include "Player.h"
#include "Match.h"
#include "GameSession.h"


Player::Player(int index, int row, int column, int maxWeight, int sessionID, string nickname)
	: Inventory(index, row, column, maxWeight), state_(ProjectJ::MatchPlayerState::NONE), score_(0), sessionID_(sessionID), nickname_(nickname)
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

		playerInfo->set_state(state_);
	}

	player->set_account_id(GetID());
	player->set_nickname(GetNickname());

	playerInfo->set_player_index(index_);
	playerInfo->set_allocated_player(player);
	playerInfo->set_allocated_position(position);
	playerInfo->set_allocated_rotation(rotation);

	return playerInfo;
}
