#include "pch.h"
#include "Client.h"

int main()
{
	ServerPacketHandler::Init();

	auto service = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 55141),
		[=]() { return make_shared<ClientSession>(); },
		10
	);

	vector<thread> threads;
	if (service->Init())
	{
		for (int i = 0; i < 1; i++)
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

	auto sessions = service->GetSessions();
	int size = sessions.size();
	for (int i = 0; i < size; i++)
	{
		sessions[i]->TestCreateRoom();
	}

	for (thread& t : threads)
	{
		t.join();
	}
}
