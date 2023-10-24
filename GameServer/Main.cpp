#include "pch.h"
#include "DBConnection.h"
#include "GameService.h"
#include "GameSession.h"
#include "GamePacketHandler.h"
#include "LogHelper.h"
#include "TickTask.h"

void DoWorkThread(const shared_ptr<GameService>& service)
{
	static atomic<int> threadIdGen = 1;
	LThreadId = threadIdGen.fetch_add(1);
	while (true)
	{
		service->GetIocpMain()->WorkThread(10);
		GTickTaskManager->DoTask();
	}
}

class TestTick : public TickTask
{
public:
	void Tick(double deltaTime) override
	{
		cout << LThreadId << ": " << deltaTime << endl;
	}
};

int main()
{
	if (false == GDBConnection->Connect(
		L"Driver={ODBC Driver 17 for SQL Server}; Server=localhost; Database=ProjectJ; UID=admin; PWD=q1w2e3r4;"))
	{
		GLogHelper->WriteStdOut(LogCategory::Log_ERROR, L"Failed to DB Connection\n");
		return 0;
	}


	GamePacketHandler::Init();
	auto tt = TickTaskManager::MakeTask<TestTick>();
	vector<thread> threads;
	auto service = make_shared<GameService>(NetAddress(L"0.0.0.0", 3000),
	                                        [=]() { return make_shared<GameSession>(); },
	                                        1000);
	if (service->Init())
	{
		GLogHelper->WriteStdOut(LogCategory::Log_INFO, L"Game Server Start\n");
		for (int i = 0; i < thread::hardware_concurrency() * 2; i++)
		{
			threads.push_back(thread([&]()
			{
				DoWorkThread(service);
			}));
		}
	}

	while (true)
	{
		ProjectJ::S_VERIFY_TOKEN pkt;
		pkt.set_result(true);
		auto sendBuffer = GamePacketHandler::MakeSendBuffer(pkt);
		this_thread::sleep_for(1s);
		service->Broadcast(sendBuffer);
	}

	for (thread& t : threads)
	{
		t.join();
	}
}
