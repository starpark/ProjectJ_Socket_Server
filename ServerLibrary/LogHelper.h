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
	BUFFER_SIZE = 512
};

struct LogWrapper
{
	LogCategory category = LogCategory::Log_TEMP;
	string buffer;
};

class LogHelper
{
public:
	LogHelper();
	~LogHelper();

public:
	void Reserve(LogCategory category, const WCHAR* format, ...);
	void Reserve(LogCategory category, const char* format, ...);
	void WriteStdOut(LogCategory category, const WCHAR* format, ...);
	void WriteStdOut(LogCategory category, const char* format, ...);
	void Write();

protected:
	void SetColor(LogCategory category);


private:
	USE_LOCK;
	HANDLE stdOut_;
	ofstream logFile_;
	queue<LogWrapper> logQueue_;
};
