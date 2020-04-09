#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include "CircularBuffer.hpp"

namespace IPEndPoint {
	class CIPEndPoint;
}

namespace TcpSocket {
	class CTcpSocket {
	public:
		explicit CTcpSocket(size_t defaultBufferSize);
		explicit CTcpSocket(SOCKET hSocket, size_t defaultBufferSize) noexcept;
		~CTcpSocket() noexcept;

	public:
		bool Bind(const class IPEndPoint::CIPEndPoint& bindAddress) noexcept;
		bool Listen(int backLog) noexcept;
		bool Connect(const class IPEndPoint::CIPEndPoint& connectAddress) noexcept;
		bool Accept(const CTcpSocket& listenSocket) noexcept;
		bool PreReceive();
		bool PostReceive();
		bool Send(const char* const buffer);

	public:
		__forceinline SOCKET GetSocketHandle() const { return mSock; }

	private:
		SOCKET mSock;
		Buffer::CCircularBuffer mReceiveBuffer;

	};

}