#pragma once
#include "Socket.h"

namespace IOCP {
	class CSession : public TcpSocket::CTcpSocket {
		using CTcpSocket::Receive;
	public:
		CSession() noexcept;

	public:
		bool Accept(const CTcpSocket& listenSocket) noexcept;
		bool PreReceive() noexcept;
		bool PostReceive() noexcept;

	private:

	};

}