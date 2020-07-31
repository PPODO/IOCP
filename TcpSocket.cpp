#include "TcpSocket.h"
#include "SocketAddress.h"
#include "overlapped.h"

TcpSocket::TcpSocket() : mhSocket(INVALID_SOCKET) {
	mhSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mhSocket == INVALID_SOCKET) {
		std::cout << "Socket Initialize Failure! - " << WSAGetLastError() << std::endl;
		throw std::bad_alloc();
	}
}

TcpSocket::~TcpSocket() {
	if (mhSocket != INVALID_SOCKET) {
		closesocket(mhSocket);
	}
}

bool TcpSocket::Bind(const SocketAddress& bindAddress) {
	if (bind(mhSocket, &bindAddress, SocketAddress::Size) == SOCKET_ERROR) {
		std::cout << "Bind Failure! - " << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

bool TcpSocket::Connect(const SocketAddress& connectAddress) {
	if (connect(mhSocket, &connectAddress, SocketAddress::Size) == SOCKET_ERROR) {
		std::cout << "Connect Failure! - " << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

bool TcpSocket::Listen(unsigned int backLogCount) {
	if (listen(mhSocket, backLogCount)) {
		std::cout << "Listen Failure! - " << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

bool TcpSocket::Accept(const TcpSocket& ListenSocket) {
	auto acceptOverlapped = new Overlapped_Accpet(this);

	if(AcceptEx(ListenSocket.mhSocket, mhSocket, acceptOverlapped->mWSABuffer.buf, 0, SocketAddress::Size + 16, SocketAddress::Size + 16, &acceptOverlapped->mReceiveBytes, &acceptOverlapped->mOverlapped) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "Accept Failure! - " << WSAGetLastError() << std::endl;
			DeleteOverlapped(acceptOverlapped);
			return false;
		}
	}
	return true;
}

bool TcpSocket::Receive() {
	DWORD flag = 0;
	auto receiveOverlapped = new Overlapped_Receive;

	if (WSARecv(mhSocket, &receiveOverlapped->mWSABuffer, 1, &receiveOverlapped->mReceiveBytes, &flag, &receiveOverlapped->mOverlapped, NULL)) {
		if (WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
			std::cout << "Receive Failure! - " << WSAGetLastError() << std::endl;
			return false;
		}
	}
	return true;
}

bool TcpSocket::Send() {
	return false;
}

bool TcpSocket::TransmitSocket() {
	auto disconnectOverlapped = new Overlapped_Disconnect;
	if (!TransmitFile(mhSocket, NULL, 0, 0, &disconnectOverlapped->mOverlapped, NULL, TF_DISCONNECT | TF_REUSE_SOCKET)) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "TransmitSocket Failure! - " << WSAGetLastError() << std::endl;
			return false;
		}
	}
	return true;
}
