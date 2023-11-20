#include "pch.h"
#include "Global.h"
#include "SendBuffer.h"
#include "DBConnectionPool.h"
#include "SocketUtils.h"
#include "LogHelper.h"
#include "DeadLockProfiler.h"
#include "TickTask.h"
#include "ThreadManager.h"
#include "CommandTask.h"
#include "TimerTask.h"

SendBufferManager* GSendBufferManager = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;
LogHelper* GLogHelper = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
TickTaskManager* GTickTaskManager = nullptr;
ThreadManager* GThreadManager = nullptr;
CommandTaskManager* GCommandTaskManager = nullptr;
TimerTaskManager* GTimerTaskManager = nullptr;

class Global
{
public:
	Global()
	{
		GSendBufferManager = new SendBufferManager();
		GDBConnectionPool = new DBConnectionPool();
		GLogHelper = new LogHelper();
		GDeadLockProfiler = new DeadLockProfiler();
		GTickTaskManager = new TickTaskManager();
		GThreadManager = new ThreadManager();
		GCommandTaskManager = new CommandTaskManager();
		GTimerTaskManager = new TimerTaskManager();

		SocketUtils::Init();
	}

	~Global()
	{
		delete GSendBufferManager;
		delete GDBConnectionPool;
		delete GLogHelper;
		delete GDeadLockProfiler;
		delete GTickTaskManager;
		delete GThreadManager;
		delete GCommandTaskManager;
		delete GTimerTaskManager;

		SocketUtils::Clear();
	}
} Global;
