#pragma once

#include "SendBuffer.h"
#include "TickTask.h"

extern thread_local shared_ptr<SendBufferBlock> LSendBufferBlock;
extern thread_local std::stack<int> LLockStack;
extern thread_local UINT32 LThreadID;
extern thread_local UINT64 LLastTick;
