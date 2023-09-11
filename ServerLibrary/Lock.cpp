#include "pch.h"
#include "Lock.h"

void Lock::TryLock()
{
	const int64_t beginTick = GetTickCount64();
	const bool desired = true;
	while (true)
	{
		for (uint32_t spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			bool expected = false;
			if (lock_.compare_exchange_strong(expected, desired))
			{
				return;
			}

			if (GetTickCount64() - beginTick >= TIMEOUT_TICK)
			{
				CRASH("LOCK TIMEOUT");
			}

			this_thread::yield();
		}
	}
}

void Lock::Unlock()
{
	lock_.store(false);
}
