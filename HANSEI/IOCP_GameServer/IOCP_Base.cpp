#include "IOCP_Base.h"
#include <iostream>

IOCP_Base::IOCP_Base() {
}

IOCP_Base::~IOCP_Base() {
	if (m_Server) {
		delete m_Server;
		m_Server = nullptr;
	}
}

bool IOCP_Base::Init(const int Port) {
	m_Server = new Socket(Port);
	if (!m_Server) {
		return false;
	}

	return true;
}

void IOCP_Base::StartIOCP() {
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	
	if (!CreateWorkerThread()) {
		return;
	}

	std::cout << "Server : Start Server!!\n";

	SOCKET ClientSocket;
	SOCKETINFO* SocketInfo;
	DWORD Flags = 0, RecvBytes = 0;
	while (true) {
		if (!m_Server->AcceptClientSocket(ClientSocket)) {
			return;
		}

		SocketInfo = new SOCKETINFO;
		memset(SocketInfo, 0, sizeof(SOCKETINFO));
		SocketInfo->m_DataBuffer.len = MaxMessageBufferSize;
		SocketInfo->m_DataBuffer.buf = SocketInfo->m_MessageBuffer;
		SocketInfo->m_Socket = ClientSocket;

		m_hIOCP = CreateIoCompletionPort((HANDLE)ClientSocket, m_hIOCP, (DWORD)SocketInfo, 0);

		if (WSARecv(SocketInfo->m_Socket, &SocketInfo->m_DataBuffer, 1, &RecvBytes, &Flags, &SocketInfo->m_Overlapped, nullptr) == INVALID_SOCKET) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				std::cout << "WSA Recv Error - " << WSAGetLastError() << std::endl;
				return;
			}
		}
	}
}

void IOCP_Base::Send(SOCKETINFO* Info) {
	DWORD SendBytes;

	memset(&Info->m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	if (WSASend(Info->m_Socket, &Info->m_DataBuffer, 1, &SendBytes, 0, nullptr, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Send Error - " << WSAGetLastError() << std::endl;
			return;
		}
	}
}

void IOCP_Base::Send(SOCKETINFO* Info, std::stringstream& SendStream) {
	DWORD SendBytes;

	memset(&Info->m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	CopyMemory(Info->m_MessageBuffer, SendStream.str().c_str(), SendStream.str().length());
	Info->m_DataBuffer.len = ULONG(SendStream.str().length());
	Info->m_DataBuffer.buf = Info->m_MessageBuffer;

	if (WSASend(Info->m_Socket, &Info->m_DataBuffer, 1, &SendBytes, 0, nullptr, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Send Error - " << WSAGetLastError() << std::endl;
			return;
		}
	}
}

void IOCP_Base::Recv(SOCKETINFO* Info) {
	DWORD RecvBytes = 0, Flags = 0;

	memset(&Info->m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	memset(Info->m_MessageBuffer, 0, MaxMessageBufferSize);
	Info->m_DataBuffer.len = MaxMessageBufferSize;
	Info->SendBytes = Info->RecvBytes = 0;

	if (WSARecv(Info->m_Socket, &Info->m_DataBuffer, 1, &RecvBytes, &Flags, &Info->m_Overlapped, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Recv Error - " << WSAGetLastError() << std::endl;
			return;
		}
	}
}