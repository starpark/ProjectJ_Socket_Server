#include "pch.h"
#include "Global.h"
#include "SendBuffer.h"
#include "DBConnection.h"
#include "SocketUtils.h"
#include "LogHelper.h"
#include "DeadLockProfiler.h"
#include "TickTask.h"

SendBufferManager* GSendBufferManager = nullptr;
DBConnection* GDBConnection = nullptr;
LogHelper* GLogHelper = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
TickTaskManager* GTickTaskManager = nullptr;

class Global
{
public:
	Global()
	{
		GSendBufferManager = new SendBufferManager();
		GDBConnection = new DBConnection();
		GLogHelper = new LogHelper();
		GDeadLockProfiler = new DeadLockProfiler();
		GTickTaskManager = new TickTaskManager();
		SocketUtils::Init();
	}

	~Global()
	{
		delete GSendBufferManager;
		delete GDBConnection;
		delete GLogHelper;
		delete GDeadLockProfiler;
		delete GTickTaskManager;
		SocketUtils::Clear();
	}
} Global;
