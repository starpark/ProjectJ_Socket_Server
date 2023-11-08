#pragma once

extern thread_local shared_ptr<class SendBufferBlock> LSendBufferBlock;
extern thread_local std::stack<int> LLockStack;
extern thread_local UINT32 LThreadID;
extern thread_local UINT64 LLastTick;
extern thread_local UINT64 LEndTickCount;
extern thread_local class CommandTaskObject* LCurrentCommandTaskObject;
