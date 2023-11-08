#include "pch.h"
#include "Client.h"

#define SESSION_COUNT 50

int main(int argc, char* argv[])
{
	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	auto service = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 3000),
		[=]() { return make_shared<ClientSession>(); },
		SESSION_COUNT
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

	auto sessions = service->GetSessionsRef();
	vector<shared_ptr<ClientSession>> cSessions;

	int index = strtol(argv[1], NULL, 10);
	for (auto session : sessions)
	{
		auto cSession = static_pointer_cast<ClientSession>(session);
		cSession->TestCreateAccount("MyTest", index);
		cSession->TestLoginHttp("MyTest", index);
		if (cSession->token.empty() == false)
		{
			cSession->TestVerifyToken();
		}
		cSessions.push_back(cSession);
		this_thread::sleep_for(50ms);
		++index;
	}

	vector<thread> testThread;


	while (true)
	{
		for (int i = 0; i < cSessions.size() / 4; i++)
		{
			cSessions[i * 4]->TestCreateRoom("Test Room" + to_string(i));
			this_thread::sleep_for(3s);
			int roomId = cSessions[i * 4]->roomID;
			cSessions[i * 4 + 1]->TestEnterRoom(roomId);
			cSessions[i * 4 + 2]->TestEnterRoom(roomId);
			cSessions[i * 4 + 3]->TestEnterRoom(roomId);

			this_thread::sleep_for(3s);
			cSessions[i * 4]->TestRoomReady();
			cSessions[i * 4 + 1]->TestRoomReady();
			cSessions[i * 4 + 3]->TestRoomReady();

			this_thread::sleep_for(5s);
		}

		this_thread::sleep_for(10s);

		for (int i = 0; i < cSessions.size(); i++)
		{
			cSessions[i]->TestLeaveRoom();

			this_thread::sleep_for(50ms);
		}
	}

	/*
	threads.push_back(thread([&]()
	{
		while (true)
		{
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
				sessions[i * 4]->Disconnect();

				this_thread::sleep_for(5s);
				sessions[i * 4]->Connect();

				this_thread::sleep_for(5s);
				sessions[i * 4]->TestLoginHttp("MyTest", i * 4);
				sessions[i * 4]->TestVerifyToken();

				this_thread::sleep_for(5s);
			}
		}
	}));*/

	/*
	threads.push_back(thread([&]()
	{
		while (true)
		{
			int size = 35;
			for (int i = 5; i < size / 3; i++)
			{
				sessions[i * 3]->TestCreateRoom(((i * 3) % 2 == 0 ? u8"�׽�Ʈ�� �� " : "Test Room ") + to_string(i + 1));
				this_thread::sleep_for(2s);
				int roomId = sessions[i * 3]->roomID;
				sessions[i * 3 + 1]->TestEnterRoom(roomId);
				sessions[i * 3 + 2]->TestEnterRoom(roomId);

				this_thread::sleep_for(2s);
				sessions[i * 3]->TestRoomReady();
				sessions[i * 3 + 1]->TestRoomReady();
				sessions[i * 3 + 2]->TestRoomReady();

				for (int j = 0; j < 10; j++)
				{
					static int count = 0;
					string message = (count & 1) == 0 ? u8"�׽�Ʈ �޽��� " : u8"test message " + sessions[i]->nickname;
					sessions[i]->TestRoomChat(message + to_string(count));
					this_thread::sleep_for(1s);
				}

				this_thread::sleep_for(2s);
				sessions[i * 3]->TestRoomReady();
				sessions[i * 3 + 1]->TestRoomReady();
				sessions[i * 3 + 2]->TestRoomReady();
			}
		}
	}));*/


	/*threads.push_back(thread([&]()
		{
			while (true)
			{
				for (int i = 0; i < 5; i++)
				{
					sessions[i]->TestLobbyChat(u8"fnhasdifbschbauysdvbasuyd");
					sessions[i]->TestRefreshRoomList();
					this_thread::sleep_for(2s);
				}
			}
		}));*/

	/*
	for (int i = 0; i < size / 3; i++)
	{
		sessions[i * 3]->TestCreateRoom(((i * 3) % 2 == 0 ? u8"�׽�Ʈ�� �� " : "Test Room ") + to_string(i + 1));
		this_thread::sleep_for(2s);
		int roomId = sessions[i * 3]->roomID;
		sessions[i * 3 + 1]->TestEnterRoom(roomId);
		sessions[i * 3 + 2]->TestEnterRoom(roomId);

		this_thread::sleep_for(2s);

		sessions[i * 3]->TestRoomReady();
		sessions[i * 3 + 2]->TestRoomReady();
	}

	int count = 1;
	while (true)
	{
		for (int i = 0; i < size; i++)
		{
			string message = (count & 1) == 0 ? u8"�׽�Ʈ �޽���" : u8"test message";
			sessions[i]->TestRoomChat(message + to_string(count));
		}
		count++;
		this_thread::sleep_for(10s);
	}
	*/

	for (thread& t : threads)
	{
		t.join();
	}

	for (thread& t : testThread)
	{
		t.join();
	}
}
