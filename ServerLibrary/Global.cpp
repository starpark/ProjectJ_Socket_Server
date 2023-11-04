#include "pch.h"
#include "Global.h"
#include "SendBuffer.h"
#include "DBConnection.h"
#include "SocketUtils.h"
#include "LogHelper.h"
#include "DeadLockProfiler.h"
#include "TickTask.h"
#include "ThreadManager.h"
#include "CommandTask.h"
#include "TimerTask.h"

SendBufferManager* GSendBufferManager = nullptr;
DBConnection* GDBConnection = nullptr;
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
		GDBConnection = new DBConnection();
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
		delete GDBConnection;
		delete GLogHelper;
		delete GDeadLockProfiler;
		delete GTickTaskManager;
		delete GThreadManager;
		delete GCommandTaskManager;
		delete GTimerTaskManager;

		SocketUtils::Clear();
	}
} Global;
