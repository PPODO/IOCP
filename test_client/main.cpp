#include <Net/Socket.h>
#include <Net/SocketAddress.h>

int main() {
	WSADATA WinSockData;
	WSAStartup(WINSOCK_VERSION, &WinSockData);

	TcpSocket::CTcpSocket Sock(1024, 0);

	Sock.Connect(IPEndPoint::CIPEndPoint("127.0.0.1", 3550));
	Sleep(1000);

	send(Sock.GetSocketHandle(), "HINO", 4, 0);

	while (true);
	WSACleanup();
	return 0;
}