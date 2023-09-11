#include "pch.h"
#include "ThreadLocal.h"

thread_local shared_ptr<SendBufferBlock> LSendBufferBlock = nullptr;
