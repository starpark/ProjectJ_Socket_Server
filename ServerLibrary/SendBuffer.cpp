#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(shared_ptr<SendBufferBlock> owner, BYTE* buffer, int size)
	: owner_(owner), buffer_(buffer), size_(size)
{
}

void SendBuffer::Close(uint32_t writeSize)
{
	ASSERT_CRASH(size_ >= writeSize);
	owner_->CloseSendBuffer(writeSize);
}

SendBufferBlock::SendBufferBlock()
{
}

SendBufferBlock::~SendBufferBlock()
{
}

void SendBufferBlock::Init()
{
	bIsAvailable_ = false;
	usedSize_ = 0;
}

shared_ptr<SendBuffer> SendBufferBlock::GetSendBuffer(uint32_t allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_BLOCK_SIZE);
	ASSERT_CRASH(bIsAvailable_ == false);

	if (allocSize > FreeSize())
	{
		return nullptr;
	}

	bIsAvailable_ = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), GetBuffer(), allocSize);
}

void SendBufferBlock::CloseSendBuffer(uint32_t writeSize)
{
	ASSERT_CRASH(bIsAvailable_ == true);
	bIsAvailable_ = false;
	usedSize_ += writeSize;
}


shared_ptr<SendBuffer> SendBufferManager::GetSendBuffer(uint32_t size)
{
	if (LSendBufferBlock == nullptr)
	{
		LSendBufferBlock = PopBlock();
		LSendBufferBlock->Init();
	}

	ASSERT_CRASH(LSendBufferBlock->IsAvailable() == false);

	if (LSendBufferBlock->FreeSize() < size)
	{
		LSendBufferBlock = PopBlock();
		LSendBufferBlock->Init();
	}

	return LSendBufferBlock->GetSendBuffer(size);
}

shared_ptr<SendBufferBlock> SendBufferManager::PopBlock()
{
	{
		LockGuard guard(lock_);
		if (sendBufferBlocks_.empty() == false)
		{
			shared_ptr<SendBufferBlock> sendBufferBlock = sendBufferBlocks_.top();
			sendBufferBlocks_.pop();

			return sendBufferBlock;
		}
	}

	return shared_ptr<SendBufferBlock>(new SendBufferBlock(), PushGlobal);
}

void SendBufferManager::PushBlock(shared_ptr<SendBufferBlock> block)
{
	LockGuard guard(lock_);
	sendBufferBlocks_.push(block);
}

void SendBufferManager::PushGlobal(SendBufferBlock* block)
{
}
