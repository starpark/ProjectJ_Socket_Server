#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

NetAddress::NetAddress(wstring ip, uint16_t port)
{
	memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = IpToAddress(ip.c_str());
	_sockAddr.sin_port = htons(port);
}

wstring NetAddress::GetIpAddress()
{
	WCHAR buffer[100];
	InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, sizeof(buffer));
	return wstring(buffer);
}

IN_ADDR NetAddress::IpToAddress(const WCHAR* ip)
{
	IN_ADDR address;
	InetPtonW(AF_INET, ip, &address);
	return address;
}
