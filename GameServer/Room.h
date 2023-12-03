#pragma once

#include "Message.pb.h"

class Player;
class GameSession;
class Lobby;
class Match;

#define MAX_PLAYER_NUMBER 4
#define INVALID_ROOM_ID -1
#define INVALID_SLOT_INDEX -1

enum class RoomState : uint8_t
{
	INVALID = 0,
	WAITING = 1,
	INGAME = 2,
};

class Room : public CommandTaskObject
{
	enum
	{
		STANDBY_COUNT = 5
	};

public:
	Room(int roomNumber, string title, shared_ptr<GameSession> hostSession, shared_ptr<Lobby> lobby);
	~Room() override;

public:
	int GetID() { return roomID_; }
	int GetNumberOfPlayer() { return numberOfPlayers_; }
	string GetTitle() { return title_; }
	vector<pair<shared_ptr<GameSession>, bool>> GetRoomInfo();
	RoomState GetState() { return state_; }
	shared_ptr<Lobby> GetLobby() { return lobby_.lock(); }
	shared_ptr<Room> GetRoomPtr() { return static_pointer_cast<Room>(shared_from_this()); }
	int GetSlotIndex(shared_ptr<GameSession> session);

	ProjectJ::RoomInfo* MakeRoomInfo();

	void SetState(RoomState state) { state_ = state; }

	int Enter(shared_ptr<GameSession> session);
	int Leave(shared_ptr<GameSession> session, int slotIndex);
	void SessionReadyToReceive(shared_ptr<GameSession> session);
	void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	void BroadcastWithoutSelf(shared_ptr<SendBuffer> sendBuffer, shared_ptr<GameSession> self);
	bool ChangePlayerPosition(shared_ptr<GameSession> session, int currentNumber, int desireNumber);
	void ToggleReady(shared_ptr<GameSession> session, int slotIndex);
	bool CheckAllReady();
	void StandByMatch(int count);
	void StartMatch();
	void EndMatch();
	void DestroyMatch();

private:
	RoomState state_ = RoomState::WAITING;
	int roomID_ = -1;
	int numberOfPlayers_ = 0;
	string title_;
	weak_ptr<Lobby> lobby_;
	shared_ptr<Match> match_;
	// 0: Chaser, 1-3: Fugitive
	vector<pair<shared_ptr<GameSession>, bool>> sessionSlots_;
	TimerHandle standbyTimerHandle_;
};
