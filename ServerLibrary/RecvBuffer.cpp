#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
	: writePos_(0), readPos_(0)
{
	buffer_.resize(MAX_BUFFER_SIZE);
}

void RecvBuffer::CleanupBuffer()
{
	int dataSize = GetDataSize();
	if (dataSize == 0)
	{
		writePos_ = readPos_ = 0;
	}
	else
	{
		if (GetFreeSize() <= MIN_BUFFER_SIZE)
		{
			memcpy(&buffer_[0], GetWriteBufferPos(), dataSize);
			readPos_ = 0;
			writePos_ = dataSize;
		}
	}
}

bool RecvBuffer::ReadData(unsigned int size)
{
	if (GetDataSize() < size)
	{
		return false;
	}

	readPos_ += size;
	return true;
}

bool RecvBuffer::WriteData(unsigned int size)
{
	if (GetFreeSize() < size)
	{
		return false;
	}

	writePos_ += size;
	return true;
}
