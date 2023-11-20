#pragma once
#include "DBConnection.h"

class DBConnectionPool
{
public:
	~DBConnectionPool();

	bool Connect(int connectionCount, const WCHAR* connectionString);
	void Clear();

	DBConnection* Pop();
	void Push(DBConnection* conn);

private:
	USE_LOCK;
	wstring connectionString_;
	SQLHENV env_ = SQL_NULL_HANDLE;
	vector<DBConnection*> connections_;
};
