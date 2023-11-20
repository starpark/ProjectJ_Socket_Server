#include "pch.h"
#include "DBConnection.h"


bool DBConnection::Connect(SQLHENV henv, const WCHAR* connectionString)
{
	if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &connection_) == SQL_ERROR)
	{
		return false;
	}

	SQLWCHAR stringBuffer[MAX_PATH] = {0};
	wcscpy_s(stringBuffer, connectionString);

	SQLWCHAR resultString[MAX_PATH] = {0};
	SQLSMALLINT resultStringLen = 0;

	SQLRETURN ret = SQLDriverConnectW(
		connection_,
		NULL,
		stringBuffer,
		_countof(stringBuffer),
		resultString,
		_countof(resultString),
		&resultStringLen,
		SQL_DRIVER_NOPROMPT
	);

	if (SQLAllocHandle(SQL_HANDLE_STMT, connection_, &statement_) != SQL_SUCCESS)
	{
		return false;
	}

	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

void DBConnection::Clear()
{
	if (connection_ != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, connection_);
		connection_ = SQL_NULL_HANDLE;
	}

	if (statement_ != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, statement_);
		statement_ = SQL_NULL_HANDLE;
	}
}

bool DBConnection::Execute(const WCHAR* qurey)
{
	SQLRETURN ret = SQLExecDirectW(statement_, (SQLWCHAR*)qurey, SQL_NTSL);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		return true;
	}

	HandleError(ret);

	return false;
}

bool DBConnection::Fetch()
{
	SQLRETURN ret = SQLFetch(statement_);

	switch (ret)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		return true;
	case SQL_NO_DATA:
		return false;
	case SQL_ERROR:
		HandleError(ret);
		return false;
	default:
		return true;
	}
}

int DBConnection::GetRowCount()
{
	SQLLEN count = 0;
	SQLRETURN ret = SQLRowCount(statement_, OUT &count);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		return static_cast<int>(count);
	}

	return -1;
}

void DBConnection::Unbind()
{
	SQLCloseCursor(statement_);
	SQLFreeStmt(statement_, SQL_UNBIND);
	SQLFreeStmt(statement_, SQL_RESET_PARAMS);
}

bool DBConnection::BindParam(int paramIndex, bool* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, sizeof(bool), value, index);
}

bool DBConnection::BindParam(int paramIndex, float* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_FLOAT, SQL_REAL, 0, value, index);
}

bool DBConnection::BindParam(int paramIndex, double* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_DOUBLE, SQL_DOUBLE, 0, value, index);
}

bool DBConnection::BindParam(int paramIndex, __int8* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, sizeof(__int8), value, index);
}

bool DBConnection::BindParam(int paramIndex, __int16* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_SHORT, SQL_SMALLINT, sizeof(__int16), value, index);
}

bool DBConnection::BindParam(int paramIndex, int* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_LONG, SQL_INTEGER, sizeof(int), value, index);
}

bool DBConnection::BindParam(int paramIndex, __int64* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_SBIGINT, SQL_BIGINT, sizeof(__int64), value, index);
}

bool DBConnection::BindParam(int paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
	*index = sizeof(SQL_TIMESTAMP_STRUCT);
	SQLRETURN ret = SQLBindParameter(statement_, paramIndex, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TYPE_TIMESTAMP, 27, 7, value, 0, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

bool DBConnection::BindParam(int paramIndex, const WCHAR* str, SQLLEN* index)
{
	SQLULEN size = (wcslen(str) + 1) * 2;
	*index = SQL_NTSL;

	if (size > WVARCHAR_MAX)
	{
		return BindParam(paramIndex, SQL_C_WCHAR, SQL_WLONGVARCHAR, size, (SQLPOINTER)str, index);
	}
	return BindParam(paramIndex, SQL_C_WCHAR, SQL_WVARCHAR, size, (SQLPOINTER)str, index);
}

bool DBConnection::BindParam(int paramIndex, const BYTE* bin, int size, SQLLEN* index)
{
	if (bin == nullptr)
	{
		*index = SQL_NULL_DATA;
		size = 1;
	}
	else
	{
		*index = size;
	}

	if (size > BINARY_MAX)
	{
		return BindParam(paramIndex, SQL_C_BINARY, SQL_LONGVARBINARY, size, (BYTE*)bin, index);
	}
	return BindParam(paramIndex, SQL_C_BINARY, SQL_BINARY, size, (BYTE*)bin, index);
}

bool DBConnection::BindCol(int columnIndex, bool* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_TINYINT, sizeof(bool), value, index);
}

bool DBConnection::BindCol(int columnIndex, float* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_FLOAT, sizeof(float), value, index);
}

bool DBConnection::BindCol(int columnIndex, double* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_DOUBLE, sizeof(double), value, index);
}

bool DBConnection::BindCol(int columnIndex, __int8* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_TINYINT, sizeof(__int8), value, index);
}

bool DBConnection::BindCol(int columnIndex, __int16* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_SHORT, sizeof(__int16), value, index);
}

bool DBConnection::BindCol(int columnIndex, int* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_LONG, sizeof(int), value, index);
}

bool DBConnection::BindCol(int columnIndex, __int64* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_SBIGINT, sizeof(__int64), value, index);
}

bool DBConnection::BindCol(int columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_TYPE_TIMESTAMP, SQL_TIMESTAMP_LEN, value, index);
}

bool DBConnection::BindCol(int columnIndex, WCHAR* str, int size, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_WCHAR, size, str, index);
}

bool DBConnection::BindCol(int columnIndex, BYTE* bin, int size, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_BINARY, size, bin, index);
}

bool DBConnection::BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index)
{
	SQLRETURN ret = SQLBindParameter(statement_, paramIndex, SQL_PARAM_INPUT, cType, sqlType, len, 0, ptr, 0, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

bool DBConnection::BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value,
                           SQLLEN* index)
{
	SQLRETURN ret = SQLBindCol(statement_, columnIndex, cType, value, len, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

void DBConnection::HandleError(SQLRETURN ret)
{
	if (ret == SQL_SUCCESS)
	{
		return;
	}

	SQLSMALLINT index = 1;
	SQLWCHAR sqlState[MAX_PATH] = {0};
	SQLINTEGER nativeErr = 0;
	SQLWCHAR errMsg[MAX_PATH] = {0};
	SQLSMALLINT msgLen = 0;
	SQLRETURN errorRet = 0;

	while (true)
	{
		errorRet = SQLGetDiagRecW(
			SQL_HANDLE_STMT,
			statement_,
			index,
			sqlState,
			OUT &nativeErr,
			errMsg,
			_countof(errMsg),
			OUT &msgLen
		);

		if (errorRet == SQL_NO_DATA)
		{
			break;
		}

		if (errorRet != SQL_SUCCESS && errorRet != SQL_SUCCESS_WITH_INFO)
		{
			break;
		}

		GLogHelper->Print(LogCategory::LOG_ERROR, L"%s\n", errMsg);

		index++;
	}
}
