#include "IOCP_Base.h"
#include <iostream>

IOCP_Base::IOCP_Base(const int Port) : m_Socket(nullptr), m_hIOCP(nullptr) {
	m_Socket = new Socket(Port);
}

IOCP_Base::~IOCP_Base() {
	if (m_Socket) {
		delete m_Socket;
		m_Socket = nullptr;
	}
	m_hIOCP = nullptr;
}

void IOCP_Base::Start() {
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

	if (!CreateWorkerThread()) {
		std::cout << "Server : Create Worker Thread Failure!\n";
		return;
	}

	std::cout << "Server : Start Login Server!!\n";

	SOCKETINFO* SocketInfo = nullptr;
	SOCKET ClientSocket = 0;
	DWORD Flags = 0, RecvBytes = 0;
	while (true) {
		if (!m_Socket->AcceptClientSocket(ClientSocket)) {
			return;
		}

		SocketInfo = new SOCKETINFO;
		memset(SocketInfo, 0, sizeof(SOCKETINFO));
		SocketInfo->m_Socket = ClientSocket;
		SocketInfo->m_DataBuffer.len = MaxMessageBuffer;
		SocketInfo->m_DataBuffer.buf = SocketInfo->m_MessageBuffer;

		m_hIOCP = CreateIoCompletionPort((HANDLE)ClientSocket, m_hIOCP, (DWORD)SocketInfo, 0);

		if (WSARecv(ClientSocket, &SocketInfo->m_DataBuffer, 1, &RecvBytes, &Flags, &SocketInfo->m_Overlapped, nullptr) == INVALID_SOCKET) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				std::cout << "WSA Recv Error! - " << WSAGetLastError() << std::endl;
				break;
			}
		}
	}
}

bool IOCP_Base::Init() {
	if (!m_Socket) {
		return false;
	}
	return true;
}

bool IOCP_Base::Recv(SOCKETINFO * SocketInfo) {
	DWORD RecvBytes = 0, Flags = 0;

	memset(&SocketInfo->m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	memset(SocketInfo->m_MessageBuffer, 0, MaxMessageBuffer);
	SocketInfo->m_DataBuffer.len = MaxMessageBuffer;
	SocketInfo->m_SendBytes = SocketInfo->m_RecvBytes = 0;

	if (WSARecv(SocketInfo->m_Socket, &SocketInfo->m_DataBuffer, 1, &RecvBytes, &Flags, &SocketInfo->m_Overlapped, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Recv Error! - " << WSAGetLastError() << std::endl;
			return false;
		}
	}
	return true;
}

bool IOCP_Base::Send(SOCKETINFO * SocketInfo, std::stringstream & SendStream) {
	DWORD SendBytes = 0, Flags = 0;

	memset(&SocketInfo->m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	memset(SocketInfo->m_MessageBuffer, 0, MaxMessageBuffer);
	CopyMemory(SocketInfo->m_MessageBuffer, SendStream.str().c_str(), SendStream.str().length());
	SocketInfo->m_DataBuffer.len = SendStream.str().length();
	SocketInfo->m_DataBuffer.buf = SocketInfo->m_MessageBuffer;

	if (WSASend(SocketInfo->m_Socket, &SocketInfo->m_DataBuffer, 1, &SendBytes, Flags, nullptr, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Send Error! - " << WSAGetLastError() << std::endl;
			return false;
		}
	}
	return true;
}