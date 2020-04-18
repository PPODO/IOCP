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
		explicit CTcpSocket(size_t defaultBufferSize, DWORD dwFlag);
		~CTcpSocket() noexcept;

	public:
		bool Bind(const class IPEndPoint::CIPEndPoint& bindAddress) noexcept;
		bool Listen(int backLog) noexcept;
		bool Connect(const class IPEndPoint::CIPEndPoint& connectAddress) noexcept;
		bool Receive() noexcept;
		bool Send(const char* const buffer) noexcept;

	public:
		void ReceiveCompletion(DWORD dwReceiveBytes) noexcept;

	public:
		__forceinline SOCKET GetSocketHandle() const { return mSock; }

	protected:
		Buffer::CCircularBuffer mReceiveBuffer;

	private:
		SOCKET mSock;

	};

}