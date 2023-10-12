#pragma once

#include "SessionBase.h"

enum class SessionState : uint8_t
{
	// before connection
	WAIT_CONNECTING = 0,
	// after connection
	LOBBY_BEFORE_LOGIN,
	LOBBY_AFTER_LOGIN,
	IN_GAME,
};

class GameSession : public SessionBase
{
public:
	GameSession();
	~GameSession() override;

public: // Getter Setter
	shared_ptr<GameSession> GetGameSessionPtr() { return static_pointer_cast<GameSession>(shared_from_this()); }
	SessionState GetState() const { return state_; }

	void SetState(SessionState state) { state_ = state; }

protected: // 콘텐츠 코드 재정의
	void OnConnected() override;
	void OnDisconnect() override;
	int OnRecv(BYTE* buffer, int numOfBytes) override;
	void OnSend(int numOfBytes) override;

protected: // 콘텐츠
	SessionState state_;
};
