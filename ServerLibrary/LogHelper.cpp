#include "pch.h"
#include "LogHelper.h"
#include <ctime>
#include <string>

LogHelper::LogHelper()
{
	namespace fs = std::filesystem;
	_wsetlocale(LC_ALL, L"korean");
	logFile_.imbue(locale("Korean"));

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

void LogHelper::Print(LogCategory category, const WCHAR* format, ...)
{
	LogWrapper log;
	va_list list;

	va_start(list, format);
	log.category = category;

	WCHAR buffer[BUFFER_SIZE]{0,};
	vswprintf_s(buffer, BUFFER_SIZE, format, list);

	log.buffer = buffer;

	va_end(list);

	logQueue_.Push(log);
}

void LogHelper::Write()
{
	if (logQueue_.IsEmpty() == false)
	{
		LogWrapper log = logQueue_.Pop();

		SetColor(log.category);

		wstring logString = L"";

		auto now = std::chrono::system_clock::now();
		auto nowTimeT = std::chrono::system_clock::to_time_t(now);

		tm timeInfo;
		localtime_s(&timeInfo, &nowTimeT);

		wchar_t date[64];
		wcsftime(date, 64, L"[%Y-%m-%d %H:%M:%S]", &timeInfo);

		logString += date;

		switch (log.category)
		{
		case LogCategory::LOG_INFO:
			logString += L"[Info] ";
			break;
		case LogCategory::LOG_TEMP:
			logString += L"[Temp] ";
			break;
		case LogCategory::LOG_ERROR:
			logString += L"[Error] ";
			break;
		case LogCategory::LOG_WARN:
			logString += L"[Warn] ";
			break;
		case LogCategory::LOG_SUCCESS:
			logString += L"[Success] ";
			break;
		}

		logString += log.buffer;

#ifdef _DEBUG
		fwprintf_s(stdout, logString.c_str());
		fflush(stdout);
#endif

		SetColor(LogCategory::LOG_INFO);

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

	SetColor(LogCategory::LOG_INFO);
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
