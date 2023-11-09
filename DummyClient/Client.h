#pragma once
#include "ServerPacketHandler.h"

class ClientService;

class ClientSession : public SessionBase
{
public:
	~ClientSession() override
	{
		cout << "~Session()" << endl;
	}

	void OnConnected() override
	{
	}

	int OnRecv(BYTE* buffer, int numOfBytes) override
	{
		int processLen = 0;

		while (true)
		{
			int dataSize = numOfBytes - processLen;
			if (dataSize < sizeof(PacketHeader))
			{
				break;
			}

			PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));

			shared_ptr<SessionBase> Session = GetSessionPtr();
			ServerPacketHandler::HandlePacket(Session, &buffer[processLen], header.size);

			processLen += header.size;
		}

		return processLen;
	}

	void OnSend(int numOfBytes) override
	{
		//cout << "OnSend()" << endl;
	}

	void OnDisconnect() override
	{
		//cout << "OnDisconnect()" << endl;
	}

	void TestCreateAccount(string prefix, int index);
	void TestLoginHttp(string prefix, int index);
	void TestVerifyToken();
	void TestCreateRoom(string title);
	void TestLeaveRoom();
	void TestEnterRoom(int roomID);
	void TestLobbyChat(string msg);
	void TestRefreshRoomList();
	void TestRoomReady();
	void TestRoomChat(string msg);

	void PrintNickname()
	{
		cout << nickname << " ";
	}

	int roomID = -1;
	bool isRoom = false;
	vector<int> roomList;
	string nickname;
	string name;
	string token;
	int id;
	bool isVerified = false;
};

class ClientService : public Service
{
public:
	ClientService(NetAddress address, SessionFactory factory, uint32_t maxSessionCount = 1)
		: Service(ServiceType::CLIENT, address, factory, maxSessionCount)
	{
	}

	bool Init() override
	{
		if (sessionFactory_ == nullptr)
		{
			return false;
		}

		const int sessionCount = GetMaxSessionCount();
		for (int i = 0; i < sessionCount; i++)
		{
			shared_ptr<SessionBase> session = CreateSession();
			if (session->Connect() == false)
				return false;
		}

		return true;
	}

	vector<shared_ptr<ClientSession>> GetSessions();
	set<shared_ptr<SessionBase>>& GetSessionsRef() { return sessions_; }
};
