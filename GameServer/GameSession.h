#pragma once

#include "SessionBase.h"
#include "Player.h"

enum class SessionState : uint8_t
{
	NONE = 0,
	LOBBY = 1,
	ROOM = 2,
	INGAME = 3
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

	void SetState(SessionState state) { state_ = state; }
	void SetPlayer(const shared_ptr<Player>& player) { player_ = player; }

public:
	void ChangeState(SessionState state);

protected:
	void OnConnected() override;
	void OnDisconnect() override;
	int OnRecv(BYTE* buffer, int numOfBytes) override;
	void OnSend(int numOfBytes) override;

protected:
	SessionState state_;
	shared_ptr<Player> player_;
};
