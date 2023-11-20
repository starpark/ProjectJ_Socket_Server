#pragma once
#include <sql.h>
#include <sqlext.h>


class DBConnection
{
	enum
	{
		WVARCHAR_MAX = 4000,
		BINARY_MAX = 8000
	};

	// 외부 사용
public:
	bool Connect(SQLHENV henv, const WCHAR* connectionString);
	void Clear();
	bool Execute(const WCHAR* qurey);
	bool Fetch();
	int GetRowCount();
	void Unbind();

	// 바인딩 함수 오버로딩
public:
	bool BindParam(int paramIndex, bool* value, SQLLEN* index);
	bool BindParam(int paramIndex, float* value, SQLLEN* index);
	bool BindParam(int paramIndex, double* value, SQLLEN* index);
	bool BindParam(int paramIndex, __int8* value, SQLLEN* index);
	bool BindParam(int paramIndex, __int16* value, SQLLEN* index);
	bool BindParam(int paramIndex, int* value, SQLLEN* index);
	bool BindParam(int paramIndex, __int64* value, SQLLEN* index);
	bool BindParam(int paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool BindParam(int paramIndex, const WCHAR* str, SQLLEN* index);
	bool BindParam(int paramIndex, const BYTE* bin, int size, SQLLEN* index);

	bool BindCol(int columnIndex, bool* value, SQLLEN* index);
	bool BindCol(int columnIndex, float* value, SQLLEN* index);
	bool BindCol(int columnIndex, double* value, SQLLEN* index);
	bool BindCol(int columnIndex, __int8* value, SQLLEN* index);
	bool BindCol(int columnIndex, __int16* value, SQLLEN* index);
	bool BindCol(int columnIndex, int* value, SQLLEN* index);
	bool BindCol(int columnIndex, __int64* value, SQLLEN* index);
	bool BindCol(int columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool BindCol(int columnIndex, WCHAR* str, int size, SQLLEN* index);
	bool BindCol(int columnIndex, BYTE* bin, int size, SQLLEN* index);

	// 내부 사용, 바인딩 함수
private:
	bool BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void HandleError(SQLRETURN ret);

private:
	SQLHDBC connection_ = SQL_NULL_HANDLE;
	SQLHSTMT statement_ = SQL_NULL_HANDLE;
};

class DBTimestampConverter
{
public:
	SQL_TIMESTAMP_STRUCT operator()() const
	{
		auto now = std::chrono::system_clock::now();
		auto nowTimeT = std::chrono::system_clock::to_time_t(now);

		tm timeInfo;
		localtime_s(&timeInfo, &nowTimeT);

		SQL_TIMESTAMP_STRUCT timestamp;
		timestamp.year = static_cast<SQLSMALLINT>(timeInfo.tm_year + 1900);
		timestamp.month = static_cast<SQLSMALLINT>(timeInfo.tm_mon + 1);
		timestamp.day = static_cast<SQLSMALLINT>(timeInfo.tm_mday);
		timestamp.hour = static_cast<SQLSMALLINT>(timeInfo.tm_hour);
		timestamp.minute = static_cast<SQLSMALLINT>(timeInfo.tm_min);
		timestamp.second = static_cast<SQLSMALLINT>(timeInfo.tm_sec);
		timestamp.fraction = 0;

		return timestamp;
	}

	SQL_TIMESTAMP_STRUCT operator()(const std::chrono::system_clock::time_point& timePoint) const
	{
		auto nowTimeT = std::chrono::system_clock::to_time_t(timePoint);

		tm timeInfo;
		localtime_s(&timeInfo, &nowTimeT);

		SQL_TIMESTAMP_STRUCT timestamp;
		timestamp.year = static_cast<SQLSMALLINT>(timeInfo.tm_year + 1900);
		timestamp.month = static_cast<SQLSMALLINT>(timeInfo.tm_mon + 1);
		timestamp.day = static_cast<SQLSMALLINT>(timeInfo.tm_mday);
		timestamp.hour = static_cast<SQLSMALLINT>(timeInfo.tm_hour);
		timestamp.minute = static_cast<SQLSMALLINT>(timeInfo.tm_min);
		timestamp.second = static_cast<SQLSMALLINT>(timeInfo.tm_sec);
		timestamp.fraction = 0;

		return timestamp;
	}
};
