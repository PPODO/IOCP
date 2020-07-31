#pragma once
#include "utils.h"

class SocketAddress;

class TcpSocket {
public:
	explicit TcpSocket();
	~TcpSocket();

public:
	bool Bind(const SocketAddress& bindAddress);
	bool Connect(const SocketAddress& connectAddress);
	bool Listen(unsigned int backLogCount = SOMAXCONN);
	bool Accept(const TcpSocket& ListenSocket);

	bool Receive();
	bool Send();

	bool TransmitSocket();

	inline SOCKET GetSocketHandle() const { return mhSocket; }

private:
	SOCKET mhSocket;

};