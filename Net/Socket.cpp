#include "Socket.h"
#include "SocketAddress.h"
#include "OverlappedType.h"

using namespace IPEndPoint;

namespace TcpSocket {
	CTcpSocket::CTcpSocket(size_t defaultBufferSize, DWORD dwFlag) : mReceiveBuffer(defaultBufferSize) {
		mSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, dwFlag);
		if (mSock == INVALID_SOCKET) {
			
		}
	}

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

	bool CTcpSocket::Receive() noexcept {
		auto receiveOverlapped = new Overlapped::Receive_Overlapped(mReceiveBuffer.GetWriteOnlyBuffer(), mReceiveBuffer.GetFreeSpaceSize());

		if (WSARecv(GetSocketHandle(), &receiveOverlapped->mWSABuf, 1, &receiveOverlapped->mUsedBytes, &receiveOverlapped->mFlag, &receiveOverlapped->mOverlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(receiveOverlapped);
				return false;
			}
		}
		return true;
	}

	bool CTcpSocket::Send(const char* const buffer) noexcept {
		auto sendOverlapped = new Overlapped::Send_Overlapped(buffer, strlen(buffer));

		if (WSASend(mSock, &sendOverlapped->mWSABuf, 1, &sendOverlapped->mUsedBytes, 0, &sendOverlapped->mOverlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(sendOverlapped);
				return false;
			}
		}
		return true;
	}

	void CTcpSocket::ReceiveCompletion(DWORD dwReceiveBytes) noexcept {
		mReceiveBuffer.Commit(dwReceiveBytes);
		std::cout << mReceiveBuffer.GetReadOnlyBuffer() << std::endl;
		mReceiveBuffer.Remove(dwReceiveBytes);
	}

}