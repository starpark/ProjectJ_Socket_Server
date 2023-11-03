#include "pch.h"
#include "Client.h"

int main()
{
	ServerPacketHandler::Init();

	auto service = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 3000),
		[=]() { return make_shared<ClientSession>(); },
		8
	);

	vector<thread> threads;
	if (service->Init())
	{
		for (int i = 0; i < 2; i++)
		{
			threads.push_back(thread([=]()
			{
				while (true)
				{
					service->GetIocpMain()->WorkThread();
				}
			}));
		}
	}

	this_thread::sleep_for(5s);

	while (true)
	{
		auto sessions = service->GetSessions();
		int size = sessions.size();
		for (int i = 0; i < size; i++)
		{
			if (i % 4 == 0)
			{
				sessions[i]->TestCreateRoom();
			}
		}

		this_thread::sleep_for(5s);
		for (int i = 0; i < size; i++)
		{
			if (i % 4 != 0)
			{
				sessions[i]->TestEnterRoom(sessions[i / 4]->roomID);
			}
		}
		this_thread::sleep_for(1s);
		for (auto session : sessions)
		{
			session->TestLeaveRoom();
		}

		this_thread::sleep_for(1s);
	}

	for (thread& t : threads)
	{
		t.join();
	}
}
