#include "pch.h"
#include "Global.h"
#include "SendBuffer.h"
#include "DBConnection.h"
#include "SocketUtils.h"
#include "LogHelper.h"

SendBufferManager* GSendBufferManager = nullptr;
DBConnection* GDBConnection = nullptr;
LogHelper* GLogHelper = nullptr;

class Global
{
public:
	Global()
	{
		GSendBufferManager = new SendBufferManager();
		GDBConnection = new DBConnection();
		GLogHelper = new LogHelper();
		SocketUtils::Init();
	}

	~Global()
	{
		delete GSendBufferManager;
		delete GDBConnection;
		delete GLogHelper;
		SocketUtils::Clear();
	}
} GServerGlobal;
