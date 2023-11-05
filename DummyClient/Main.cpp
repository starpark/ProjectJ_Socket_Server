#include "pch.h"
#include "Client.h"

int main()
{
	ServerPacketHandler::Init();

	auto service = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 55141),
		[=]() { return make_shared<ClientSession>(); },
		4
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

	this_thread::sleep_for(3s);

	auto sessions = service->GetSessions();
	int size = sessions.size();
	for (int i = 0; i < size; i++)
	{
		sessions[i]->TestCreateAccount("MyTest", i);
		sessions[i]->TestLoginHttp("MyTest", i);
		if (sessions[i]->token.empty() == false)
		{
			sessions[i]->TestVerifyToken();
		}
	}

	for (int i = 0; i < size / 4; i++)
	{
		sessions[i * 4]->TestCreateRoom("Test Room" + to_string(i));
		this_thread::sleep_for(3s);
		int roomId = sessions[i * 4]->roomID;
		sessions[i * 4 + 1]->TestEnterRoom(roomId);
		sessions[i * 4 + 2]->TestEnterRoom(roomId);
		sessions[i * 4 + 3]->TestEnterRoom(roomId);

		this_thread::sleep_for(3s);
		sessions[i * 4]->TestRoomReady();
		sessions[i * 4 + 1]->TestRoomReady();
		sessions[i * 4 + 2]->TestRoomReady();
		sessions[i * 4 + 3]->TestRoomReady();

		this_thread::sleep_for(10s);
		sessions[i * 4 + 2]->Disconnect();
	}

	for (thread& t : threads)
	{
		t.join();
	}
}
