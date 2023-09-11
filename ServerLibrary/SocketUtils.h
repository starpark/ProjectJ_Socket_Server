#pragma once
#include "NetAddress.h"

class SocketUtils
{
public:
	~SocketUtils();
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;
public:
	static void Init();
	static void Clear();

	static bool SetLinger(SOCKET socket, uint16_t onoff, uint16_t linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int size);
	static bool SetSendBufferSize(SOCKET socket, int size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static bool Bind(SOCKET socket, NetAddress netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16_t port);
	static bool Listen(SOCKET socket, int backlog = SOMAXCONN);

	static SOCKET CreateSocket();
	static void CloseSocket(SOCKET& socket);
};

template <typename T>
static inline bool SetSockOpt(SOCKET socket, int level, int optName, T optVal)
{
	return SOCKET_ERROR != setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}
