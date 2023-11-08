#pragma once

#include "SessionBase.h"
#include "Player.h"
#include "Inventory.h"

class Lobby;
class Room;
class Match;

enum class SessionState : uint8_t
{
	NONE = 0,
	LOBBY = 1,
	ROOM = 2,
	MATCH = 3
};

class GameSession : public SessionBase
{
public:
	GameSession();
	~GameSession() override;

public:
	shared_ptr<GameSession> GetGameSessionPtr() { return static_pointer_cast<GameSession>(shared_from_this()); }
	SessionState GetState() { return state_; }
	shared_ptr<Player>& GetPlayer() { return player_; }
	int GetID() { return id_; }
	string GetNickname() { return nickname_; }
	string GetName() { return name_; }
	bool IsVerified() { return bIsVerified; }

	shared_ptr<Lobby> TryGetLobby()
	{
		if (state_ == SessionState::LOBBY) return lobby_.lock();
		return nullptr;
	}

	shared_ptr<Room> TryGetRoom()
	{
		if (state_ == SessionState::ROOM) return room_.lock();
		return nullptr;
	}

	shared_ptr<Match> TryGetMatch()
	{
		if (state_ == SessionState::MATCH) return match_.lock();
		return nullptr;
	}

	void ProcessEnterLobby(const shared_ptr<Lobby>& lobby);
	void ProcessLeaveLobby();

	void ProcessEnterRoom(const shared_ptr<Room>& room);
	void ProcessLeaveRoom();

	void ProcessEnterMatch(const shared_ptr<Match>& match, const shared_ptr<Player>& player);
	void ProcessLeaveMatch();

	void SetID(int id) { id_ = id; }
	void SetNickname(string& nickname) { nickname_ = nickname; }
	void SetNickname(string&& nickname) { nickname_ = std::move(nickname); }
	void SetName(string& name) { name_ = name; }
	void SetName(string&& name) { name_ = std::move(name); }
	void SetState(SessionState state) { state_ = state; }
	void SetPlayer(const shared_ptr<Player>& player) { player_ = player; }
	void SetLobby(const shared_ptr<Lobby>& lobby) { lobby_ = lobby; }
	void SetRoom(const shared_ptr<Room>& room) { room_ = room; }
	void SetMatch(const shared_ptr<Match>& match) { match_ = match; }
	void SetIsVerified(bool verifying) { bIsVerified = verifying; }

	ProjectJ::Player* MakePlayer();

protected:
	void OnConnected() override;
	void OnDisconnect() override;
	int OnRecv(BYTE* buffer, int numOfBytes) override;
	void OnSend(int numOfBytes) override;


protected:
	atomic<SessionState> state_ = SessionState::NONE;
	bool bIsVerified = false;
	int id_ = -1;
	string nickname_;
	string name_;
	shared_ptr<Player> player_;
	weak_ptr<Lobby> lobby_;
	weak_ptr<Room> room_;
	weak_ptr<Match> match_;
};

/*
class GameSessionMananger
{
public:
	void Add(shared_ptr<GameSession> session);
	void Remove(shared_ptr<GameSession> session);
	void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	void Broadcast(SessionState state, shared_ptr<SendBuffer> sendBuffer);

private:
	USE_LOCK;
	set<shared_ptr<GameSession>> sessions_;
};

extern GameSessionMananger GSessionManager;
*/
