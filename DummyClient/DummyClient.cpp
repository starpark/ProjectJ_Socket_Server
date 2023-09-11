#include "pch.h"
#include "Service.h"
#include "SessionBase.h"
#include "ServerPacketHandler.h"

class ClientService : public Service
{
public:
	ClientService(NetAddress address, SessionFactory factory, uint32_t maxSessionCount = 1)
		: Service(ServiceType::CLIENT, address, factory, maxSessionCount)
	{
	}

	~ClientService() override
	{
	}

	void Start() override
	{
		for (int i = 0; i < GetMaxSessionCount(); i++)
		{
			auto session = CreateSession();
			if (session->Connect() == false)
			{
				cout << 1 << endl;
				return;
			}
		}
	}
};

class ServerSession : public SessionBase
{
public:
	~ServerSession() override
	{
		cout << "~ServerSession" << endl;
	}

	void OnConnected() override
	{
	}

	void OnDisconnect() override
	{
	}

	void OnSend(int len) override
	{
		cout << "OnSend Len = " << len << endl;
	}

	int OnRecv(BYTE* buffer, int numOfBytes) override
	{
		cout << buffer << endl;
		return numOfBytes;
	}
};

int main()
{
	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	auto service = make_shared<ClientService>(
		NetAddress(L"192.168.50.94", 3000), [=]() { return make_shared<ServerSession>(); }, 1);

	service->Start();

	thread t1([&service]() { service->GetIocpMain()->WorkThread(); }), t2([&service]()
	{
		service->GetIocpMain()->WorkThread();
	});

	auto sb = GSendBufferManager->GetSendBuffer(1);
	char tt = 'c';
	memcpy(sb->GetBuffer(), &tt, 1);
	sb->Close(1);

	while (true)
	{
		this_thread::sleep_for(1s);
		service->BroadCastWithoutSelf(nullptr, sb);
	}

	t1.join();
	t2.join();
}
