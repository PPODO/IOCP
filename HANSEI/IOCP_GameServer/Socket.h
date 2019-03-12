#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

static const size_t MaxLen = 13;

class Socket {
private:
	WSADATA m_WinSockData;
	SOCKET m_Listener;
	SOCKADDR_IN m_Address;
	int m_SocketAddressSize;

private:
	IN_ADDR GetIPv4Address();

public:
	Socket(const int Port);
	~Socket();

public:
	bool AcceptClientSocket(SOCKET& ClientSocket);

};