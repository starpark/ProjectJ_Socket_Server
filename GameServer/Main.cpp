#include "pch.h"
#include "DBConnection.h"
#include "GameService.h"
#include "GameSession.h"
#include "GamePacketHandler.h"
#include "LogHelper.h"

int main()
{
	if (false == GDBConnection->Connect(
		L"Driver={ODBC Driver 17 for SQL Server}; Server=localhost; Database=ProjectJ; UID=admin; PWD=q1w2e3r4;"))
	{
		GLogHelper->WriteStdOut(LogCategory::Log_ERROR, L"Failed to DB Connection\n");
		return 0;
	}

	GamePacketHandler::Init();

	vector<thread> threads;
	auto service = make_shared<GameService>(NetAddress(L"127.0.0.1", 3000),
	                                        [=]() { return make_shared<GameSession>(); },
	                                        1000);
	if (service->Init())
	{
		GLogHelper->WriteStdOut(LogCategory::Log_INFO, L"Game Server Start\n");
		for (int i = 0; i < thread::hardware_concurrency() * 2; i++)
		{
			threads.push_back(thread([=]()
			{
				service->Start();
			}));
		}
	}

	auto sb = GSendBufferManager->GetSendBuffer(1);
	char tt = 'c';
	memcpy(sb->GetBuffer(), &tt, 1);
	sb->Close(1);
	int count = 1;
	while (true)
	{
		protocol::S_LEAVE pkt;
		pkt.set_id(count);
		count++;
		auto sendBuffer = GamePacketHandler::MakeSendBuffer(pkt);
		this_thread::sleep_for(1s);
		service->BroadCastWithoutSelf(nullptr, sendBuffer);
	}

	for (thread& t : threads)
	{
		t.join();
	}
}
