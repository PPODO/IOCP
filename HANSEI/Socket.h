#pragma once
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>

class Socket {
private:
	WSADATA WinSockData;
	SOCKET m_Socket;
	SOCKADDR_IN m_Address;

public:
	Socket();
	bool Init(const short Port);
	bool AcceptToClient(Socket& SocketClass, int AddrLen = sizeof(SOCKADDR_IN));

	bool DestroySocket();
	~Socket();

public:
	inline SOCKET GetSocket() const { return m_Socket; }

};