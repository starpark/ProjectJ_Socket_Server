#include "pch.h"
#include "DBConnection.h"
#include "GameService.h"
#include "GameSession.h"
#include "GamePacketHandler.h"
#include "Item.h"

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
		GTickTaskManager->DoTask(currentTick);
		GCommandTaskManager->Execute();
		GTimerTaskManager->Execute();
	}
}

class TestTick : public TickTask
{
public:
	int count = 0;
	double timeElapsed = 0.0f;

	void Tick(double deltaTime) override
	{
		count++;
		timeElapsed += deltaTime;
		double fps = static_cast<double>(count) / timeElapsed;
		GLogHelper->Reserve(LogCategory::Log_WARN, "%d: %d %lf\n", LThreadID, count, fps);
	}
};


int main()
{
	if (false == GDBConnection->Connect(
		L"Driver={ODBC Driver 17 for SQL Server}; Server=localhost; Database=ProjectJ; UID=admin; PWD=q1w2e3r4;"))
	{
		GLogHelper->Reserve(LogCategory::Log_ERROR, L"DB Connection Failure\n");
		return 0;
	}
	GLogHelper->Reserve(LogCategory::Log_SUCCESS, L"DB Connection Success\n");

	ItemManager::Init();
	GamePacketHandler::Init();
	vector<thread> threads;
	auto service = make_shared<GameService>(NetAddress(L"0.0.0.0", 3000),
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
