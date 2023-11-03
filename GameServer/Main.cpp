#include "pch.h"
#include "DBConnection.h"
#include "GameService.h"
#include "GameSession.h"
#include "GamePacketHandler.h"
#include "LogHelper.h"
#include "TickTask.h"
#include "Inventory.h"

void DoWorkThread(const shared_ptr<GameService>& service)
{
	static atomic<int> threadIdGen = 1;
	LThreadID = threadIdGen.fetch_add(1);

	while (true)
	{
		const UINT64 currentTick = GetTickCount64();

		service->GetIocpMain()->WorkThread(10);
		GTickTaskManager->DoTask(currentTick);
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
		// cout << LThreadID << ": " << deltaTime << " " << count << " " << fps << " " << endl;
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


	GamePacketHandler::Init();
	auto tt1 = TickTaskManager::MakeTask<TestTick>();
	auto tt2 = TickTaskManager::MakeTask<TestTick>();
	vector<thread> threads;
	auto service = make_shared<GameService>(NetAddress(L"0.0.0.0", 3000),
	                                        [=]() { return make_shared<GameSession>(); },
	                                        1000);
	if (service->Init())
	{
		GLogHelper->Reserve(LogCategory::Log_SUCCESS, L"Successfully Start The ProjectJ Server\n");
		for (int i = 0; i < thread::hardware_concurrency() * 2; i++)
		{
			threads.push_back(thread([&]()
			{
				DoWorkThread(service);
			}));
		}
	}
	else
	{
		GLogHelper->Reserve(LogCategory::Log_ERROR, L"The ProjectJ Server Failed to Start\n");
	}

	while (true)
	{
		GLogHelper->Write();
	}

	for (thread& t : threads)
	{
		t.join();
	}
}
