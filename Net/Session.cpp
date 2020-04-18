#pragma comment(lib, "mswsock.lib")
#include "Session.h"
#include "Config.h"
#include "SocketAddress.h"
#include "OverlappedType.h"
#include <MSWSock.h>

using namespace IPEndPoint;

namespace IOCP {
	CSession::CSession() noexcept : CTcpSocket(GetNetConfig().mDefaultBufferSize, WSA_FLAG_OVERLAPPED) {}

	bool CSession::Accept(const CTcpSocket& listenSocket) noexcept {
		auto acceptOverlapped = new Overlapped::Accept_Overlapped(this);

		if (AcceptEx(listenSocket.GetSocketHandle(), GetSocketHandle(), &acceptOverlapped->mOutBuffer, 0, CIPEndPoint::GetSize() + 16, CIPEndPoint::GetSize() + 16, &acceptOverlapped->mUsedBytes, &acceptOverlapped->mOverlapped) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(acceptOverlapped);
				std::cout << "Error\n";
				return false;
			}
		}
		return true;
	}

	bool CSession::PreReceive() noexcept {
		auto receiveZeroOverlapped = new Overlapped::ReceiveZero_Overlapped(nullptr, 0);

		if (WSARecv(GetSocketHandle(), &receiveZeroOverlapped->mWSABuf, 1, &receiveZeroOverlapped->mUsedBytes, &receiveZeroOverlapped->mFlag, &receiveZeroOverlapped->mOverlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
				Overlapped::DeleteIOContext(receiveZeroOverlapped);
				std::cout << "Error! - " << WSAGetLastError() << std::endl;
				return false;
			}
		}
		return true;
	}

	bool CSession::PostReceive() noexcept {
		return CTcpSocket::Receive();
	}

}