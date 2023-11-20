#include "pch.h"
#include "DBConnectionPool.h"

DBConnectionPool::~DBConnectionPool()
{
}

bool DBConnectionPool::Connect(int connectionCount, const WCHAR* connectionString)
{
	WRITE_LOCK;

	connectionString_ = connectionString;

	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env_) != SQL_SUCCESS)
	{
		return false;
	}

	if (SQLSetEnvAttr(env_, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
	{
		return false;
	}

	for (int i = 0; i < connectionCount; i++)
	{
		auto connection = new DBConnection();
		if (connection->Connect(env_, connectionString) == false)
		{
			delete connection;
			return false;
		}

		connections_.push_back(connection);
	}

	return true;
}

void DBConnectionPool::Clear()
{
	WRITE_LOCK;

	if (env_ != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, env_);
		env_ = SQL_NULL_HANDLE;
	}

	for (DBConnection* connection : connections_)
		delete connection;

	connections_.clear();
}

DBConnection* DBConnectionPool::Pop()
{
	WRITE_LOCK;

	if (connections_.empty())
	{
		auto newDBConn = new DBConnection();
		if (newDBConn->Connect(env_, connectionString_.c_str()) == false)
		{
			return nullptr;
		}
		return newDBConn;
	}

	DBConnection* connection = connections_.back();
	connections_.pop_back();
	return connection;
}

void DBConnectionPool::Push(DBConnection* connection)
{
	WRITE_LOCK;
	connections_.push_back(connection);
}
