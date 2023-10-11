#pragma once

class SendBuffer
{
public:
	SendBuffer(shared_ptr<class SendBufferBlock> owner, BYTE* buffer, int size);

public:
	BYTE* GetBuffer() { return buffer_; }
	uint32_t GetSize() { return size_; }

	void Close(uint32_t writeSize);

private:
	shared_ptr<SendBufferBlock> owner_;
	BYTE* buffer_;
	uint32_t size_;
};

class SendBufferBlock : public enable_shared_from_this<SendBufferBlock>
{
	enum
	{
		SEND_BUFFER_BLOCK_SIZE = 0x2000
	};

public:
	SendBufferBlock();
	~SendBufferBlock();

public:
	void Init();
	shared_ptr<SendBuffer> GetSendBuffer(uint32_t allocSize);
	void CloseSendBuffer(uint32_t writeSize);

	bool IsAvailable() { return bIsAvailable_; }
	BYTE* GetBuffer() { return &buffer_[usedSize_]; }
	uint32_t FreeSize() { return static_cast<uint32_t>(buffer_.size()) - usedSize_; }

private:
	array<BYTE, SEND_BUFFER_BLOCK_SIZE> buffer_ = {};
	bool bIsAvailable_ = false;
	uint32_t usedSize_ = 0;
};

class SendBufferManager
{
public: // 외부 사용
	shared_ptr<SendBuffer> GetSendBuffer(uint32_t size);

private:
	shared_ptr<SendBufferBlock> PopBlock();
	void PushBlock(shared_ptr<SendBufferBlock> block);
	static void PushGlobal(SendBufferBlock* block);

private:
	USE_LOCK;
	stack<shared_ptr<SendBufferBlock>> sendBufferBlocks_;
	BYTE* currentChunk_;
	int currentUseSize_;
};
