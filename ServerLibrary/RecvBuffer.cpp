#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
	: head_(0), tail_(0)
{
	buffer_.resize(MAX_BUFFER_SIZE);
}

void RecvBuffer::CleanupBuffer()
{
	if (head_ == tail_)
	{
		head_ = tail_ = 0;
	}
	else
	{
		if (GetFreeSize() <= MIN_BUFFER_SIZE)
		{
			const unsigned int dataSize = GetDataSize();
			memcpy(&buffer_[0], GetBufferHead(), dataSize);
			tail_ = 0;
			head_ = dataSize;
		}
	}
}

bool RecvBuffer::ReadData(unsigned int size)
{
	if (GetDataSize() < size)
	{
		return false;
	}

	tail_ += size;
	return true;
}

bool RecvBuffer::WriteData(unsigned int size)
{
	if (GetFreeSize() < size)
	{
		return false;
	}

	head_ += size;
	return true;
}
