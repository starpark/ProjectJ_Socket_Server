#include "pch.h"
#include "Global.h"
#include "SendBuffer.h"
#include "DBConnection.h"
#include "SocketUtils.h"
#include "LogHelper.h"
#include "DeadLockProfiler.h"

SendBufferManager* GSendBufferManager = nullptr;
DBConnection* GDBConnection = nullptr;
LogHelper* GLogHelper = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

class Global
{
public:
	Global()
	{
		GSendBufferManager = new SendBufferManager();
		GDBConnection = new DBConnection();
		GLogHelper = new LogHelper();
		GDeadLockProfiler = new DeadLockProfiler();
		SocketUtils::Init();
	}

	~Global()
	{
		delete GSendBufferManager;
		delete GDBConnection;
		delete GLogHelper;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
} GServerGlobal;
