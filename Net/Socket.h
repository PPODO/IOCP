#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#pragma(lib, "ws2_32.lib")
#include <WinSock2.h>

namespace IPEndPoint {
	class CIPEndPoint;
}

namespace TcpSocket {
	class CTcpSocket {
	public:
		explicit CTcpSocket();
		explicit CTcpSocket(SOCKET hSocket) noexcept;
		~CTcpSocket() noexcept;

	public:
		bool Bind(const class IPEndPoint::CIPEndPoint& bindAddress) noexcept;
		bool Listen(int backLog) noexcept;
		bool Connect(const class IPEndPoint::CIPEndPoint& connectAddress) noexcept;
		bool Accept(const CTcpSocket& listenSocket) noexcept;

	public:


	private:
		SOCKET mSock;

	};

}