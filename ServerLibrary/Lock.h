#pragma once

class Lock
{
	enum : uint32_t
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 3000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:
	Lock() : lockFlag_(EMPTY_FLAG), writeCount_(0)
	{
	}

	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);
	bool TryWriteLock(const char* name);

	Lock(const Lock&) = delete;
	Lock& operator=(const Lock&) = delete;
private:
	atomic<uint32_t> lockFlag_;
	uint16_t writeCount_;
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : lock_(lock), name_(name) { lock_.ReadLock(name); }
	~ReadLockGuard() { lock_.ReadUnlock(name_); }

	ReadLockGuard(const ReadLockGuard&) = delete;
	ReadLockGuard& operator=(const ReadLockGuard&) = delete;

private:
	Lock& lock_;
	const char* name_;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : lock_(lock), name_(name) { lock_.WriteLock(name); }

	WriteLockGuard(Lock& lock, const char* name, adopt_lock_t) : lock_(lock), name_(name)
	{
	}

	~WriteLockGuard() { lock_.WriteUnlock(name_); }

	WriteLockGuard(const WriteLockGuard&) = delete;
	WriteLockGuard& operator=(const WriteLockGuard&) = delete;

private:
	Lock& lock_;
	const char* name_;
};

template <typename Lock0, typename Lock1>
bool MultipleLockAttemptSmall(const char* name, Lock0& lock0, Lock1& lock1)
{
	// 2개의 락 시도. 데드락 회피 이용.
	lock0.WriteLock(name);
	if (lock1.TryWriteLock(name))
	{
		return false;
	}

	lock0.WriteUnlock(name);
	this_thread::yield();
	return true;
}

template <size_t... Indices, typename... LockN>
void LockFromLocks(const char* name, const int target, index_sequence<Indices...>, LockN&... lockN)
{
	((static_cast<int>(Indices) == target ? (void)lockN.WriteLock(name) : void()), ...);
}

template <size_t... Indices, typename... LockN>
bool TryLockFromLocks(const char* name, const int target, index_sequence<Indices...>, LockN&... lockN)
{
	bool result{};
	((static_cast<int>(Indices) == target ? (void)(result = lockN.TryWriteLock(name)) : void()), ...);

	return result;
}

template <size_t... Indices, typename... LockN>
void UnlockLocks(const char* name, const int first, const int last, index_sequence<Indices...>, LockN&... lockN)
{
	((first <= static_cast<int>(Indices) && static_cast<int>(Indices) < last ? (void)lockN.WriteUnlock(name) : void()), ...);
}

template <typename... LockN>
int TryLockRange(const char* name, const int first, const int last, LockN&... lockN)
{
	using Indices = index_sequence_for<LockN...>;
	int next = first;
	for (; next != last; ++next)
	{
		if (TryLockFromLocks(name, next, Indices{}, lockN...) == false)
		{
			UnlockLocks(name, first, next, Indices{}, lockN...);
			return next;
		}
	}

	return -1;
}

template <typename... LockN>
int MultipleLockAttemptLarge(const char* name, const int hardLock, LockN&... lockN)
{
	using Indices = index_sequence_for<LockN...>;
	LockFromLocks(name, hardLock, Indices{}, lockN...);
	int failed = -1;
	int backoutStart = hardLock;

	failed = TryLockRange(name, 0, hardLock, lockN...);
	if (failed == -1)
	{
		backoutStart = 0;
		failed = TryLockRange(name, hardLock + 1, sizeof...(LockN), lockN...);
		if (failed == -1)
		{
			return -1;
		}
	}

	UnlockLocks(name, backoutStart, hardLock + 1, Indices{}, lockN...);
	this_thread::yield();
	return failed;
}

template <typename Lock0, typename Lock1>
void MultipleLockAttempt(const char* name, Lock0& lock0, Lock1& lock1)
{
	while (MultipleLockAttemptSmall(name, lock0, lock1) && MultipleLockAttemptSmall(name, lock1, lock0))
	{
	}
}

template <typename Lock0, typename Lock1, typename Lock2, typename... LockN>
void MultipleLockAttempt(const char* name, Lock0& lock0, Lock1& lock1, Lock2& lock2, LockN& ...lockN)
{
	int hardLock = 0;
	while (hardLock != -1)
	{
		hardLock = MultipleLockAttemptLarge(name, hardLock, lock0, lock1, lock2, lockN...);
	}
}


template <typename Lock0, typename Lock1, typename... LockN>
void MultipleLock(const char* name, Lock0& lock0, Lock1& lock1, LockN&... lockN)
{
	MultipleLockAttempt(name, lock0, lock1, lockN...);
}


template <typename... Locks>
class MultipleWriteLockGuard
{
public:
	MultipleWriteLockGuard(const char* name, Locks& ... locks)
		: name_(name), locks_(locks...)
	{
		MultipleLock(name, locks...);
	}

	~MultipleWriteLockGuard()
	{
		apply([name = name_](auto&... locks)
		{
			(locks.WriteUnlock(name), ...);
		}, locks_);
	}

	MultipleWriteLockGuard(const MultipleWriteLockGuard&) = delete;
	MultipleWriteLockGuard& operator=(const MultipleWriteLockGuard&) = delete;

private:
	const char* name_;
	tuple<Locks&...> locks_;
};
