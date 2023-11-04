#include "pch.h"
#include "LogHelper.h"
#include <ctime>
#include <string>

LogHelper::LogHelper()
{
	namespace fs = std::filesystem;
	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");

	stdOut_ = GetStdHandle(STD_OUTPUT_HANDLE);
	UINT64 startTick = GetTickCount64();

	fs::path path("Log");
	path /= "LogFile_" + std::to_string(startTick) + ".txt";
	create_directory(path.parent_path());

	logFile_.open(path);
}

LogHelper::~LogHelper()
{
	logFile_.close();
}

void LogHelper::Reserve(LogCategory category, const WCHAR* format, ...)
{
	LogWrapper log;
	va_list list;

	va_start(list, format);
	log.category = category;

	WCHAR buffer[BUFFER_SIZE];
	vswprintf_s(buffer, BUFFER_SIZE, format, list);

	wstring wideString(buffer);
	log.buffer.assign(wideString.begin(), wideString.end());

	va_end(list);

	logQueue_.Push(log);
}

void LogHelper::Reserve(LogCategory category, const char* format, ...)
{
	LogWrapper log;
	va_list list;

	va_start(list, format);

	log.category = category;

	char buffer[BUFFER_SIZE];
	vsprintf_s(buffer, BUFFER_SIZE, format, list);

	log.buffer = string(buffer);

	va_end(list);

	logQueue_.Push(log);
}

void LogHelper::Write()
{
	if (logQueue_.IsEmpty() == false)
	{
		LogWrapper log = logQueue_.Pop();

		SetColor(log.category);

		string logString = "";

		time_t now = time(NULL);
		tm tm;
		localtime_s(&tm, &now);

		char date[64];
		strftime(date, 64, "[%Y-%m-%d %H:%M:%S]", &tm);

		logString += date;

		switch (log.category)
		{
		case LogCategory::Log_INFO:
			logString += "[Info] ";
			break;
		case LogCategory::Log_TEMP:
			logString += "[Temp] ";
			break;
		case LogCategory::Log_ERROR:
			logString += "[Error] ";
			break;
		case LogCategory::Log_WARN:
			logString += "[Warn] ";
			break;
		case LogCategory::Log_SUCCESS:
			logString += "[Success] ";
			break;
		}

		logString += log.buffer;

#ifdef _DEBUG
		fprintf_s(stdout, logString.c_str());
		fflush(stdout);
#endif

		SetColor(LogCategory::Log_INFO);

		if (logFile_.is_open())
		{
			logFile_ << logString << flush;
		}
	}
}

void LogHelper::WriteStdOut(LogCategory category, const WCHAR* format, ...)
{
	if (format == nullptr)
		return;

	SetColor(category);

	va_list ap;
	va_start(ap, format);
	vwprintf_s(format, ap);
	va_end(ap);

	fflush(stdout);

	SetColor(LogCategory::Log_INFO);
}

void LogHelper::WriteStdOut(LogCategory category, const char* format, ...)
{
	if (format == nullptr)
		return;

	SetColor(category);

	va_list ap;
	va_start(ap, format);
	vprintf_s(format, ap);
	va_end(ap);

	fflush(stdout);

	SetColor(LogCategory::Log_INFO);
}

void LogHelper::SetColor(LogCategory category)
{
	static WORD SColors[]
	{
		FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
		FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY
	};

	SetConsoleTextAttribute(stdOut_, SColors[static_cast<int>(category)]);
}
