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
	unsigned int GetDataSize() const { return writePos_ - readPos_; }
	unsigned int GetFreeSize() const { return MAX_BUFFER_SIZE - writePos_; }
	BYTE* GetWriteBufferPos() { return &buffer_[writePos_]; }
	BYTE* GetReadBufferPos() { return &buffer_[readPos_]; }

public: // 외부 사용
	void CleanupBuffer();
	bool ReadData(unsigned int size);
	bool WriteData(unsigned int size);

private:
	vector<BYTE> buffer_;
	unsigned int writePos_;
	unsigned int readPos_;
};
