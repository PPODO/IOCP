#pragma once
#include <string>
#include "utils.h"

class SocketAddress {
public:
	explicit SocketAddress(const std::string& ipAddress, u_short  portNumber);
	
	const sockaddr* operator&() const {
		return &mAddress;
	}
	sockaddr* operator&() {
		return const_cast<sockaddr*>(&static_cast<const SocketAddress>(*this));
	}

private:
	sockaddr_in* GetSockAddrIn() { return reinterpret_cast<sockaddr_in*>(&mAddress); }

public:
	static const size_t Size;

private:
	sockaddr mAddress;

};