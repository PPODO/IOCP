#pragma once
#pragma(lib, "ws2_32.lib")
#include <WinSock2.h>

namespace IPEndPoint {
	class CIPEndPoint;
}

namespace TcpSocket {
	class CTcpSocket {
	public:
		CTcpSocket();

	private:
		SOCKET mSock;

	};

}