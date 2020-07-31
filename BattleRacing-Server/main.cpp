#include "Iocp.h"
#include "packet.h"
#include "packet_serializer.h"
#include "memorypool.h"

int main() {
	WSADATA data;
	WSAStartup(WINSOCK_VERSION, &data);

	Iocp iocp;
	iocp.Initialize();
	while (true) {};

	WSACleanup();
	return 0;
}