#include "pch.h"
#include "Client.h"

#define TEST_PORT 55141
#define LIVE_PORT 3000
#define SESSION_COUNT 6

int main(int argc, char* argv[])
{
	this_thread::sleep_for(1s);

	ServerPacketHandler::Init();
	auto service = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", TEST_PORT),
		[=]() { return make_shared<ClientSession>(); },
		SESSION_COUNT
	);

	if (service->Init())
	{
		for (int i = 0; i < 4; i++)
		{
			GThreadManager->Launch([&]()
			{
				while (true)
				{
					service->GetIocpMain()->WorkThread();
				}
			});
		}
	}
	else
	{
		return 0;
	}

	this_thread::sleep_for(5s);
	auto sessions = service->GetSessions();

	int threadCount = 3;
	int offset = sessions.size() / threadCount;
	for (int i = 0; i < threadCount; i++)
	{
		int start = i * offset;
		int end = (i + 1) * offset;

		for (int k = start; k < end; k++)
		{
			sessions[k]->TestCreateAccount("MyTest", k);
			sessions[k]->TestLoginHttp("MyTest", k);
			this_thread::sleep_for(100ms);
			if (sessions[k]->token.empty() == false)
			{
				sessions[k]->TestVerifyToken();
			}
			else
			{
				cout << "Error" << endl;
			}
		}

		int clientPerTest = 2;
		for (int k = 0; k < offset / clientPerTest; k++)
		{
			int p = k * clientPerTest + start;

			sessions[p]->TestCreateRoom(u8"이동 동기화 테스트 방 " + to_string(p));
			this_thread::sleep_for(2s);
			int roomId = sessions[p]->roomID;
			sessions[p + 1]->TestEnterRoom(roomId);
			//sessions[p + 2]->TestEnterRoom(roomId);
			//sessions[p + 3]->TestEnterRoom(roomId);

			this_thread::sleep_for(2s);
			sessions[p]->TestRoomReady();
			sessions[p + 1]->TestRoomReady();
			//sessions[p + 2]->TestRoomReady();
			//sessions[p + 3]->TestRoomReady();
		}
	}

	/*for (auto session : sessions)
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

	int clientPerTest = 3;
	for (int i = 0; i < cSessions.size() / clientPerTest; i++)
	{
		cSessions[i * clientPerTest]->TestCreateRoom(u8"한글 테스트 방" + to_string(i));
		this_thread::sleep_for(3s);
		int roomId = cSessions[i * clientPerTest]->roomID;
		cSessions[i * clientPerTest + 1]->TestEnterRoom(roomId);
		cSessions[i * clientPerTest + 2]->TestEnterRoom(roomId);
		//cSessions[i * clientPerTest + 3]->TestEnterRoom(roomId);

		this_thread::sleep_for(3s);
		cSessions[i * clientPerTest]->TestRoomReady();
		cSessions[i * clientPerTest + 1]->TestRoomReady();
		cSessions[i * clientPerTest + 2]->TestRoomReady();
		//cSessions[i * clientPerTest + 3]->TestRoomReady();
	}*/


	/*while (true)
	{
		int clientPerTest = 3;
		for (int i = 0; i < cSessions.size() / clientPerTest; i++)
		{
			cSessions[i * clientPerTest]->TestCreateRoom(u8"한글 테스트 방" + to_string(i));
			this_thread::sleep_for(3s);
			int roomId = cSessions[i * clientPerTest]->roomID;
			cSessions[i * clientPerTest + 1]->TestEnterRoom(roomId);
			cSessions[i * clientPerTest + 2]->TestEnterRoom(roomId);

			this_thread::sleep_for(3s);
			cSessions[i * clientPerTest]->TestRoomReady();
			cSessions[i * clientPerTest + 1]->TestRoomReady();
			cSessions[i * clientPerTest + 2]->TestRoomReady();

			this_thread::sleep_for(3s);
			cSessions[i * clientPerTest]->TestRoomChat("Hi Room" + to_string(roomId) + " I am " + to_string(cSessions[i * clientPerTest]->id));
			cSessions[i * clientPerTest + 1]->
				TestRoomChat("Hi Room" + to_string(roomId) + " I am " + to_string(cSessions[i * clientPerTest + 1]->id));
			cSessions[i * clientPerTest + 2]->TestRoomChat(u8"하이요" + to_string(roomId) + u8" 난 " + to_string(cSessions[i * clientPerTest + 2]->id));

			this_thread::sleep_for(5s);
		}

		this_thread::sleep_for(10s);

		for (int i = 0; i < cSessions.size(); i++)
		{
			cSessions[i]->TestLeaveRoom();

			this_thread::sleep_for(50ms);
		}

		for (int i = 0; i < cSessions.size(); i++)
		{
			cSessions[i]->TestLobbyChat(u8"한글 채팅 테스트");

			this_thread::sleep_for(50ms);
		}
	}*/

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
				sessions[i * 3]->TestCreateRoom(((i * 3) % 2 == 0 ? u8"테스트용 방 " : "Test Room ") + to_string(i + 1));
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
					string message = (count & 1) == 0 ? u8"테스트 메시지 " : u8"test message " + sessions[i]->nickname;
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
		sessions[i * 3]->TestCreateRoom(((i * 3) % 2 == 0 ? u8"테스트용 방 " : "Test Room ") + to_string(i + 1));
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
			string message = (count & 1) == 0 ? u8"테스트 메시지" : u8"test message";
			sessions[i]->TestRoomChat(message + to_string(count));
		}
		count++;
		this_thread::sleep_for(10s);
	}
	*/

	GThreadManager->Join();
}
