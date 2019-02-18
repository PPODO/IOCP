#include "Socket.h"

Socket::Socket() {
	memset(&m_Address, 0, sizeof(SOCKADDR_IN));
}

bool Socket::Init(const short Port) {
	using std::cout;
	
	if (WSAStartup(WINSOCK_VERSION, &WinSockData) != 0) {
		cout << "Start WinSock Failure!\n";
		return false;
	}
	cout << "Start WinSock Successful\n";
	
	if ((m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		cout << "Create Socket Failure!\n";
		return false;
	}
	cout << "Create Socket Successful!\n";

	m_Address.sin_family = PF_INET;
	m_Address.sin_port = htons(Port);
	m_Address.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(m_Socket, (sockaddr*)&m_Address, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		cout << "Bind Socket Failure!\n";
		return false;
	}
	cout << "Bind Socket Successful!\n";

	if (listen(m_Socket, 5) == SOCKET_ERROR) {
		cout << "Listen Socket Successful!\n";
		return false;
	}
	cout << "Listen Socket Successful!\n";
	return true;
}

bool Socket::AcceptToClient(Socket& SocketClass, int AddrLen) {
	if ((SocketClass.m_Socket = accept(m_Socket, (sockaddr*)&SocketClass.m_Address, &AddrLen)) == INVALID_SOCKET) {
		std::cout << "Server : Accept Socket Failure!\n";
		return false;
	}
	std::cout << "Server : Accept Socket Successful! - " << SocketClass.m_Socket << std::endl;
	return true;
}

bool Socket::DestroySocket() {
	if (m_Socket) {
		closesocket(m_Socket);
		m_Socket = 0;
		return true;
	}
	return false;
}

Socket::~Socket() {
	WSACleanup();
}