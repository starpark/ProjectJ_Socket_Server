#include "pch.h"
#include "ThreadLocal.h"

thread_local shared_ptr<SendBufferBlock> LSendBufferBlock = nullptr;
thread_local std::stack<int> LLockStack;
thread_local UINT32 LThreadID = 0;
thread_local UINT64 LLastTick = 0;
thread_local UINT64 LEndTickCount = 0;
