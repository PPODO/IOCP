#include "SocketAddress.h"
#include <WS2tcpip.h>

const size_t SocketAddress::Size = sizeof(SocketAddress);

SocketAddress::SocketAddress(const std::string& ipAddress, u_short portNumber) {
	GetSockAddrIn()->sin_family = AF_INET;
	GetSockAddrIn()->sin_port = htons(portNumber);
	InetPtonA(AF_INET, ipAddress.c_str(), &GetSockAddrIn()->sin_addr);
}