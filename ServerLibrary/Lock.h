#pragma once

class Lock
{
	enum
	{
		TIMEOUT_TICK = 10000,
		// 10s
		MAX_SPIN_COUNT = 5000
	};

public:
	void TryLock();
	void Unlock();

private:
	atomic<bool> lock_ = false;
};

class LockGuard
{
public:
	LockGuard(Lock& lock) : lock_(lock) { lock.TryLock(); }
	~LockGuard() { lock_.Unlock(); }

private:
	Lock& lock_;
};
