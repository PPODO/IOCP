#pragma comment(lib, "mswsock.lib")
#include "Socket.h"
#include "SocketAddress.h"
#include <MSWSock.h>

using namespace IPEndPoint;

namespace TcpSocket {
	CTcpSocket::CTcpSocket() {
		mSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		if (mSock == INVALID_SOCKET) {
			throw std::exception("bad initialization!");
		}
	}

	CTcpSocket::CTcpSocket(SOCKET hSocket) noexcept : mSock(hSocket) {}

	CTcpSocket::~CTcpSocket() {
		closesocket(mSock);
	}

	bool CTcpSocket::Bind(const CIPEndPoint& bindAddress) noexcept {
		if (bind(mSock, &bindAddress, CIPEndPoint::GetSize()) == SOCKET_ERROR) {

			return false;
		}
		return true;
	}

	bool CTcpSocket::Listen(int backLog) noexcept {
		if (listen(mSock, backLog) == SOCKET_ERROR) {

			return false;
		}
		return true;
	}

	bool CTcpSocket::Connect(const CIPEndPoint& connectAddress) noexcept {
		if (connect(mSock, &connectAddress, CIPEndPoint::GetSize()) == SOCKET_ERROR) {

			return false;
		}
		return true;
	}

	bool CTcpSocket::Accept(const CTcpSocket& listenSocket) noexcept {
		/*if (AcceptEx(listenSocket.mSock, mSock, , 0, CIPEndPoint::GetSize() + 16, CIPEndPoint::GetSize() + 16, , ) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {

				return false;
			}
		}*/
		return true;
	}

	
}