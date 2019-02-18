#include "IOCPBase.h"

IOCPBase::IOCPBase() {
	m_hIOCP = m_hThread = nullptr;
}

bool IOCPBase::Init() {
	if (m_Server.Init(3500)) {
		return true;
	}
	return false;
}


void IOCPBase::StartIOCP() {
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (!CreateWorkerThread()) {
		return;
	}

	std::cout << "Server : Server Start!\n";

	Socket ClientSocket;
	SOCKETINFO* SocketInfo;
	DWORD RecvBytes = 0, Flags = 0;
	while (1) {
		if (!m_Server.AcceptToClient(ClientSocket)) {
			break;
		}

		SocketInfo = new SOCKETINFO;
		memset(SocketInfo, 0, sizeof(SOCKETINFO));
		SocketInfo->Socket = ClientSocket.GetSocket();
		SocketInfo->DataBuffer.len = MAX_BUFFER;
		SocketInfo->DataBuffer.buf = SocketInfo->MessageBuffer;
		SocketInfo->RecvBytes = SocketInfo->SendBytes = 0;

		m_hIOCP = CreateIoCompletionPort((HANDLE)SocketInfo->Socket, m_hIOCP, (DWORD)SocketInfo, 0);

		if (WSARecv(SocketInfo->Socket, &SocketInfo->DataBuffer, 1, &RecvBytes, &Flags, &SocketInfo->Overlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				std::cout << "WSA Recv Error - " << WSAGetLastError() << std::endl;
				break;
			}
		}
	}
}

void IOCPBase::Send(SOCKETINFO * SocketInfo) {
	DWORD SendBytes;

	memset(&SocketInfo->Overlapped, 0, sizeof(WSAOVERLAPPED));
	if (WSASend(SocketInfo->Socket, &SocketInfo->DataBuffer, 1, &SendBytes, 0, NULL, NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Send Error - " << WSAGetLastError() << std::endl;
			return;
		}
	}
}

void IOCPBase::Recv(SOCKETINFO * SocketInfo) {
	DWORD RecvBytes = 0, Flags = 0;

	memset(&SocketInfo->Overlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&SocketInfo->MessageBuffer, 0, MAX_BUFFER);
	SocketInfo->DataBuffer.len = MAX_BUFFER;
	SocketInfo->DataBuffer.buf = SocketInfo->MessageBuffer;
	if (WSARecv(SocketInfo->Socket, &SocketInfo->DataBuffer, 1, &RecvBytes, &Flags, &SocketInfo->Overlapped, NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Recv Error - " << WSAGetLastError() << std::endl;
			return;
		}
	}
}

IOCPBase::~IOCPBase() {
	if (m_hThread) {
		for (int i = 0; i < m_ThreadCount; i++) {
			CloseHandle(m_hThread[i]);
		}
		delete[] m_hThread;
		m_hThread = nullptr;
	}
	if (m_hIOCP) {
		CloseHandle(m_hIOCP);
		m_hIOCP = nullptr;
	}
}