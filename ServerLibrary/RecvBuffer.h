#pragma once

class RecvBuffer
{
	enum
	{
		MIN_BUFFER_SIZE = 0x1000,
		MAX_BUFFER_SIZE = 0x10000
	};

public:
	RecvBuffer();

public: // Getter
	unsigned int GetDataSize() const { return head_ - tail_; }
	unsigned int GetFreeSize() const { return MAX_BUFFER_SIZE - head_; }
	BYTE* GetBufferHead() { return &buffer_[head_]; }
	BYTE* GetBufferTail() { return &buffer_[tail_]; }

public: // 외부 사용
	void CleanupBuffer();
	bool ReadData(unsigned int size);
	bool WriteData(unsigned int size);

private:
	vector<BYTE> buffer_;
	unsigned int head_;
	unsigned int tail_;
};
