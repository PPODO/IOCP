#include "SocketAddress.h"
#include <WS2tcpip.h>

namespace IPEndPoint {
	CIPEndPoint::CIPEndPoint(const std::string& ipAddress, uint16_t portNumber) noexcept {
		GetAddr_In()->sin_family = AF_INET;
		GetAddr_In()->sin_port = htons(portNumber);
		InetPton(AF_INET, ipAddress.c_str(), &GetAddr_In()->sin_addr);
	}

}