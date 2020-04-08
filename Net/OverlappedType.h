#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

namespace Overlapped {
	enum class EOverlappedFlag : uint8_t {
		Accept,
		TryDisconnect,
		Disconnect,
		Receive,
		Send
	};

	struct Base_Overlapped {
	public:
		WSAOVERLAPPED mOverlapped;
		EOverlappedFlag mFlag;


	};
	
}