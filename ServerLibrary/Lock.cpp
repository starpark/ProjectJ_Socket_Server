#include "pch.h"
#include "Lock.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	const uint32_t lockThreadId = (lockFlag_.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadID == lockThreadId)
	{
		writeCount_++;
		return;
	}

	const int64_t beginTick = GetTickCount64();
	const uint32_t desired = ((LThreadID << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32_t spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32_t expected = EMPTY_FLAG;
			if (lockFlag_.compare_exchange_strong(OUT expected, desired))
			{
				writeCount_++;
				return;
			}
		}

#ifdef _DEBUG
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");
#endif

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	if ((lockFlag_.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int lockCount = --writeCount_;
	if (lockCount == 0)
		lockFlag_.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	const uint32_t lockThreadId = (lockFlag_.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadID == lockThreadId)
	{
		lockFlag_.fetch_add(1);
		return;
	}

	const int64_t beginTick = GetTickCount64();
	while (true)
	{
		for (uint32_t spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32_t expected = (lockFlag_.load() & READ_COUNT_MASK);
			if (lockFlag_.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

#ifdef _DEBUG
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");
#endif

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	if ((lockFlag_.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}

bool Lock::TryWriteLock(const char* name)
{
	const uint32_t lockThreadId = (lockFlag_.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadID == lockThreadId)
	{
		return true;
	}


	const uint32_t desired = ((LThreadID << 16) & WRITE_THREAD_MASK);
	uint32_t expected = EMPTY_FLAG;
	if (lockFlag_.compare_exchange_strong(expected, desired))
	{
#if _DEBUG
		GDeadLockProfiler->PushLock(name);
#endif
		writeCount_++;
		return true;
	}

	return false;
}
