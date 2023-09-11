#pragma once

enum class LogCategory
{
	Log_INFO,
	Log_TEMP,
	Log_ERROR,
	Log_WARN
};

class LogHelper
{
public:
	LogHelper();

public:
	void WriteStdOut(LogCategory category, const WCHAR* format, ...);

protected:
	void SetColor(bool stdOut, LogCategory category);

private:
	HANDLE stdOut_;
	HANDLE stdErr_;
};
