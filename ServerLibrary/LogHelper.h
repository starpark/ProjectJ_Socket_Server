#pragma once

#include <fstream>
#include <filesystem>

enum class LogCategory : uint8_t
{
	LOG_INFO,
	LOG_TEMP,
	LOG_ERROR,
	LOG_WARN,
	LOG_SUCCESS,
};

enum
{
	BUFFER_SIZE = 4096
};

struct LogWrapper
{
	LogCategory category = LogCategory::LOG_TEMP;
	wstring buffer;
};

class LogHelper
{
public:
	LogHelper();
	~LogHelper();

public:
	void Print(LogCategory category, const WCHAR* format, ...);
	void WriteStdOut(LogCategory category, const WCHAR* format, ...);
	void Write();

protected:
	void SetColor(LogCategory category);


private:
	HANDLE stdOut_;
	wofstream logFile_;
	LockQueue<LogWrapper> logQueue_;
};
