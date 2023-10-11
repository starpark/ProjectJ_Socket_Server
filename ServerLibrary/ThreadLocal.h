#pragma once

#include "SendBuffer.h"

extern thread_local shared_ptr<SendBufferBlock> LSendBufferBlock;
extern thread_local std::stack<int> LLockStack;
extern thread_local uint32_t LThreadId;
