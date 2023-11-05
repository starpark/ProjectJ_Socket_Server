#pragma once

class Player;
class GameSession;
class Lobby;
class Match;

#define MAX_PLAYER_NUMBER 4

enum class RoomState : uint8_t
{
	WAITING = 0,
	INGAME = 1,
};

class Room : public enable_shared_from_this<Room>
{
public:
	Room(int roomNumber, string title, shared_ptr<GameSession> hostSession, shared_ptr<Lobby> lobby);
	~Room();

public:
	int GetID() { return roomID_; }
	int GetNumberOfPlayer() { return numberOfPlayers_; }
	string GetTitle() { return title_; }
	vector<pair<shared_ptr<GameSession>, bool>> GetRoomInfo();
	RoomState GetState() { return state_; }
	shared_ptr<Lobby> GetLobby() { return lobby_.lock(); }

	void SetState(RoomState state) { state_ = state; }


	bool EnterSession(shared_ptr<GameSession> session);
	int LeaveSession(const shared_ptr<GameSession>& session);
	void BroadcastHere(shared_ptr<SendBuffer> sendBuffer);
	void BroadcastWithoutSelf(shared_ptr<SendBuffer> sendBuffer, const shared_ptr<GameSession>& self);
	bool ChangePlayerPosition(const shared_ptr<GameSession>& session, int currentNumber, int desireNumber);
	void ToggleReady(const shared_ptr<GameSession>& session);
	bool CheckAllReady();
	void StandByMatch(UINT count);
	void StartMatch();
	void EndMatch();
	void DestroyMatch();

private:
	USE_LOCK;
	atomic<bool> standby_ = false;
	RoomState state_ = RoomState::WAITING;
	int roomID_ = -1;
	int numberOfPlayers_ = 0;
	string title_;
	weak_ptr<Lobby> lobby_;
	shared_ptr<Match> match_;
	// 0: Chaser, 1-3: Fugitive
	vector<pair<shared_ptr<GameSession>, bool>> sessionSlots_;
};
