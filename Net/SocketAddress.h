#pragma once
#pragma(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <string>

namespace IPEndPoint {
	class CIPEndPoint {
	public:
		explicit CIPEndPoint(const std::string& ipAddress, uint16_t portNumber) noexcept;

	public:
		__forceinline const sockaddr* operator&() const {
			return &mAddress;
		}

		__forceinline sockaddr* operator&() {
			return &mAddress;
		}

	private:
		__forceinline sockaddr_in* GetAddr_In() { return reinterpret_cast<sockaddr_in*>(&mAddress); }

	private:
		sockaddr mAddress;

	};

}