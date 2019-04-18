#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Socket.h"
#include <string>
#include <iostream>

IN_ADDR Socket::GetIPv4Address() {
	char localhostname[MaxLen] = { "\0" };
	IN_ADDR Address{ 0, };

	try {
		if (gethostname(localhostname, MaxLen) == SOCKET_ERROR) {
			throw "Get host name is Failure!";
		}
		HOSTENT* Ptr = gethostbyname(localhostname);
		if (!Ptr) { throw "Get host name is Failure!"; }

		while (Ptr && Ptr->h_name) {
			if (Ptr->h_addrtype == PF_INET) {
				memcpy(&Address, Ptr->h_addr_list[0], Ptr->h_length);
				break;
			}
			Ptr++;
		}
		return Address;
	}
	catch (const std::string& Exception) {
		std::cout << Exception << std::endl;
	}
	return Address;
}

Socket::Socket(const int Port) : m_SocketAddressSize(sizeof(SOCKADDR_IN)) {
	memset(&m_Address, 0, sizeof(SOCKADDR_IN));
	m_Address.sin_family = PF_INET;
	m_Address.sin_port = htons(Port);

	try {
		if (WSAStartup(WINSOCK_VERSION, &m_WinSockData) < 0) {
			throw "Startup WinSock Failure!";
		}

		if ((m_Listener = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
			throw "Create Socket Failure!";
		}

		m_Address.sin_addr = GetIPv4Address();
		if (bind(m_Listener, (sockaddr*)&m_Address, m_SocketAddressSize) == SOCKET_ERROR) {
			throw "Bind Socket Failure!";
		}

		if (listen(m_Listener, 5) == SOCKET_ERROR) {
			throw "Listen Socket Failure!";
		}
	}
	catch (const char* Exception) {
		std::cout << Exception << std::endl;
	}
}

Socket::~Socket() {
	closesocket(m_Listener);
	WSACleanup();
}

bool Socket::AcceptClientSocket(SOCKET& ClientSocket) {
	SOCKADDR_IN ClientAddr;
	if ((ClientSocket = accept(m_Listener, (sockaddr*)&ClientAddr, &m_SocketAddressSize)) == SOCKET_ERROR) {
		std::cout << "Server : Accept Failure!\n";
		return false;
	}
	std::cout << "Server : " << ClientSocket << " Accept Successful! : " << ntohs(ClientAddr.sin_port) << std::endl;
	return true;
}