#pragma once

#include <fstream>
#include <filesystem>

enum class LogCategory : uint8_t
{
	Log_INFO,
	Log_TEMP,
	Log_ERROR,
	Log_WARN,
	Log_SUCCESS
};

enum
{
	BUFFER_SIZE = 4096
};

struct LogWrapper
{
	LogCategory category = LogCategory::Log_TEMP;
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
