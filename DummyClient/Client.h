#pragma once
#include "ServerPacketHandler.h"

const int accountID = 6;
const string nickname = "test100";
const string name = "test100";
const string token =
	"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJQcm9qZWN0SiBBUEkgU2VydmVyIiwiZXhwIjoxNzAxNDI1MDA2LCJwbGF5ZXJfaWQiOjYsIm5hbWUiOiJ0ZXN0MTAwIiwibmlja25hbWUiOiJ0ZXN0MTAwIn0.nJQz6rK5yVfvYcKT3PxfmBojsSvcAxdugVzeUDgNLl8";

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
		//cout << "OnConnected()" << endl;
		ProjectJ::C_VERIFY_TOKEN packet;
		packet.set_account_id(accountID);
		packet.set_token(token);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
		Send(sendBuffer);
	}

	int OnRecv(BYTE* buffer, int numOfBytes) override
	{
		//GLogHelper->WriteStdOut(LogCategory::Log_TEMP, L"OnRecv\n");
		int processLen = 0;

		while (true)
		{
			int dataSize = numOfBytes - processLen;
			if (dataSize <= sizeof(PacketHeader))
			{
				break;
			}

			PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
			if (dataSize < header.size)
			{
				break;
			}

			shared_ptr<SessionBase> Session = GetSessionPtr();
			ServerPacketHandler::HandlePacket(Session, buffer, header.size);

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

	void TestCreateRoom();
	void TestLeaveRoom();
	void TestEnterRoom(int roomID);
	void TestRefreshRoomList();

	int roomID = -1;
	bool isRoom = false;
	vector<int> roomList;
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
};
