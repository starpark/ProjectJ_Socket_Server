#include "pch.h"
#include "DBConnection.h"
#include "GameService.h"
#include "GameSession.h"
#include "GamePacketHandler.h"
#include "DataManager.h"

#define TEST_PORT 55141
#define LIVE_PORT 3000

enum
{
	TICK_SLICE = 64
};

void DoWorkThread(const shared_ptr<GameService>& service)
{
	while (true)
	{
		const UINT64 currentTick = GetTickCount64();
		LEndTickCount = currentTick + TICK_SLICE;

		service->GetIocpMain()->WorkThread(10);

		// Tasks
		GTimerTaskManager->Distribute();
		GCommandTaskManager->ExecuteTask();
	}
}

int main()
{
	if (false == GDBConnection->Connect(
		L"Driver={ODBC Driver 17 for SQL Server}; Server=localhost; Database=ProjectJ; UID=admin; PWD=q1w2e3r4;"))
	{
		GLogHelper->Print(LogCategory::Log_ERROR, L"DB Connection Failure\n");
		return 0;
	}
	GLogHelper->Print(LogCategory::Log_SUCCESS, L"DB Connection Success\n");

	if (DataManager::Init() == false)
	{
		return 0;
	}
	GamePacketHandler::Init();

	auto service = make_shared<GameService>(NetAddress(L"0.0.0.0", TEST_PORT),
	                                        [=]() { return make_shared<GameSession>(); },
	                                        1000);

	if (service->Init())
	{
		for (int i = 0; i < thread::hardware_concurrency() * 2; i++)
		{
			GThreadManager->Launch([&]()
			{
				DoWorkThread(service);
			});
		}
	}

	while (true)
	{
		GLogHelper->Write();
	}

	GThreadManager->Join();
}
