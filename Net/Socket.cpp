#pragma comment(lib, "mswsock.lib")
#include "Socket.h"
#include "SocketAddress.h"
#include "Iocp.h"
#include "OverlappedType.h"
#include <MSWSock.h>

using namespace IPEndPoint;

namespace TcpSocket {
	CTcpSocket::CTcpSocket(size_t defaultBufferSize) : mReceiveBuffer(defaultBufferSize) {
		mSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		if (mSock == INVALID_SOCKET) {
			throw std::exception("bad initialization!");
		}
	}

	CTcpSocket::CTcpSocket(SOCKET hSocket, size_t defaultBufferSize) noexcept : mReceiveBuffer(defaultBufferSize), mSock(hSocket) {}

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
		auto acceptOverlapped = new Overlapped::Accept_Overlapped(this);

		if (AcceptEx(listenSocket.mSock, mSock, &acceptOverlapped->mOutBuffer, 0, CIPEndPoint::GetSize() + 16, CIPEndPoint::GetSize() + 16, &acceptOverlapped->mUsedBytes, &acceptOverlapped->mOverlapped) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(acceptOverlapped);
				std::cout << "Error\n";
				return false;
			}
		}
		return true;
	}

	bool CTcpSocket::PreReceive() {
		auto receiveZeroOverlapped = new Overlapped::ReceiveZero_Overlapped(nullptr, 0);
		if (WSARecv(mSock, &receiveZeroOverlapped->mWSABuf, 1, &receiveZeroOverlapped->mUsedBytes, &receiveZeroOverlapped->mFlag, &receiveZeroOverlapped->mOverlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(receiveZeroOverlapped);
				std::cout << "Error! - " << WSAGetLastError() << std::endl;
				return false;
			}
		}
		return true;
	}

	bool CTcpSocket::PostReceive() {
		char Buffer[3];
		auto receiveOverlapped = new Overlapped::Receive_Overlapped(Buffer, 2);

		if (WSARecv(mSock, &receiveOverlapped->mWSABuf, 1, &receiveOverlapped->mUsedBytes, &receiveOverlapped->mFlag, &receiveOverlapped->mOverlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(receiveOverlapped);
				return false;
			}
		}
		return true;
	}

	bool CTcpSocket::Send(const char* const buffer) {
		auto sendOverlapped = new Overlapped::Send_Overlapped(buffer, strlen(buffer));

		if (WSASend(mSock, &sendOverlapped->mWSABuf, 1, &sendOverlapped->mUsedBytes, 0, &sendOverlapped->mOverlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(sendOverlapped);
				return false;
			}
		}
		return true;
	}

	
}