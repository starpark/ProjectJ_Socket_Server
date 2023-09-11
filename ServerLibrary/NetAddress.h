#pragma once

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16_t port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	wstring GetIpAddress();
	uint16_t GetPort() { return ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR IpToAddress(const WCHAR* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};
