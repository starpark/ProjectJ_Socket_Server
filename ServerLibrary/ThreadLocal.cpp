#include "pch.h"
#include "ThreadLocal.h"

thread_local shared_ptr<SendBufferBlock> LSendBufferBlock = nullptr;
thread_local std::stack<int> LLockStack;
thread_local uint32_t LThreadId = 0;
thread_local TickCounts LTickCounts;
