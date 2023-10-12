#include "pch.h"
#include "LogHelper.h"

LogHelper::LogHelper()
{
	stdOut_ = GetStdHandle(STD_OUTPUT_HANDLE);
	stdErr_ = GetStdHandle(STD_ERROR_HANDLE);
}

void LogHelper::WriteStdOut(LogCategory category, const WCHAR* format, ...)
{
	if (format == nullptr)
		return;

	SetColor(true, category);

	va_list ap;
	va_start(ap, format);
	vwprintf(format, ap);
	va_end(ap);

	fflush(stdout);

	SetColor(true, LogCategory::Log_INFO);
}

void LogHelper::WriteStdOut(LogCategory category, const char* format, ...)
{
	if (format == nullptr)
		return;

	SetColor(true, category);

	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);

	fflush(stdout);

	SetColor(true, LogCategory::Log_INFO);
}

void LogHelper::SetColor(bool stdOut, LogCategory category)
{
	static WORD SColors[]
	{
		FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
		FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
	};

	SetConsoleTextAttribute(stdOut ? stdOut_ : stdErr_, SColors[static_cast<int>(category)]);
}
