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
	bool Connect(const WCHAR* connectionString);
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
	bool BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr,
	               SQLLEN* index);
	bool BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void HandleError(SQLRETURN ret);

private:
	SQLHENV environment_ = NULL;
	SQLHDBC connection_ = NULL;
	SQLHSTMT statement_ = NULL;
};
