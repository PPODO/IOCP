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

	DWORD RecvBytes = 0, Flags = 0;
	CLIENTPACKETINFORMATION* ClientPacketInfo = nullptr;
	SOCKETINFO* SocketInfo = nullptr;
	SOCKET ClientSocket;
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

		ClientPacketInfo = new CLIENTPACKETINFORMATION(sizeof(GAMEPACKET));
		m_Clients.insert(std::make_pair(ClientSocket, ClientPacketInfo));

		Recv(SocketInfo);
	}
}

bool IOCP_Base::Init() {
	if (!m_Socket) {
		return false;
	}
	return true;
}

bool IOCP_Base::Recv(SOCKETINFO* Info) {
	DWORD RecvBytes = 0, Flags = 0;

	ZeroMemory(&Info->m_Overlapped, sizeof(WSAOVERLAPPED));
	memset(Info->m_MessageBuffer, 0, MaxMessageBuffer);
	Info->m_DataBuffer.len = sizeof(GAMEPACKET);
	Info->m_SendBytes = Info->m_RecvBytes = 0;

	if (WSARecv(Info->m_Socket, &Info->m_DataBuffer, 1, &RecvBytes, &Flags, &Info->m_Overlapped, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Recv Error! - " << WSAGetLastError() << '\t' << Info->m_Socket << std::endl;
			return false;
		}
	}
	return true;
}

bool IOCP_Base::Send(SOCKETINFO* Info, GAMEPACKET*& Packet) {
	DWORD SendBytes = 0, Flags = 0;

	ZeroMemory(&Info->m_Overlapped, sizeof(WSAOVERLAPPED));
	Info->m_DataBuffer.len = sizeof(GAMEPACKET);
	Info->m_DataBuffer.buf = (char*)Packet;
	Info->m_SendBytes = Info->m_RecvBytes = 0;

	if (WSASend(Info->m_Socket, &Info->m_DataBuffer, 1, &SendBytes, Flags, nullptr, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Send Error! - " << WSAGetLastError() << '\t' << Info->m_Socket << std::endl;
			return false;
		}
	}
	return true;
}

bool IOCP_Base::Send(SOCKETINFO* Info, GAMEPACKET& Packet) {
	DWORD SendBytes = 0, Flags = 0;

	ZeroMemory(&Info->m_Overlapped, sizeof(WSAOVERLAPPED));
	Info->m_DataBuffer.len = sizeof(GAMEPACKET);
	Info->m_DataBuffer.buf = (char*)&Packet;
	Info->m_SendBytes = Info->m_RecvBytes = 0;

	if (WSASend(Info->m_Socket, &Info->m_DataBuffer, 1, &SendBytes, Flags, nullptr, nullptr) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "WSA Send Error! - " << WSAGetLastError() << '\t' << Info->m_Socket << std::endl;
			return false;
		}
	}
	return true;
}