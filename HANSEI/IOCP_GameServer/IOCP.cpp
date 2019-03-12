#include "IOCP.h"
#include <iostream>
#include <sstream>
#include <string>

IOCP::IOCP() {
}

IOCP::~IOCP() {
	for (size_t i = 0; i < m_WorkerThread.size(); i++) {
		m_WorkerThread[i].join();
	}
	m_WorkerThread.clear();
}

void IOCP::StartIOCP() {
	IOCP_Base::StartIOCP();

}

bool IOCP::CreateWorkerThread() {
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	size_t m_ThreadCount = SystemInfo.dwNumberOfProcessors * 2;

	for (size_t i = 0; i < m_ThreadCount; i++) {
		m_WorkerThread.push_back(std::thread([this]() {
			ProcessWorkerThread();
		}));
	}
	std::cout << "Create Worker Thread Successful!\n";
	return true;
}

void IOCP::ProcessWorkerThread() {
	DWORD RecvBytes = 0, CompletionKey = 0;
	SOCKETINFO* EventSocket;

	while (true) {
		if (GetQueuedCompletionStatus(GetIOCPHandle(), &RecvBytes, (PULONG_PTR)&CompletionKey, (LPOVERLAPPED*)&EventSocket, INFINITE) == 0) {
			std::unique_lock<std::mutex> Lock(m_Lock);
			if (WSAGetLastError() != ERROR_NETNAME_DELETED) {
				std::cout << "Get Queued Completion Status Error!! - " << WSAGetLastError() << std::endl;
				return;
			}
		}

		if (RecvBytes == 0) {
			std::cout << "Server : Disconnet Socket - " << EventSocket->m_Socket << std::endl;
			if (EventSocket) {
				closesocket(EventSocket->m_Socket);
				delete EventSocket;
			}
			continue;
		}

		try {
			std::stringstream RecvStream(EventSocket->m_DataBuffer.buf);
			int PacketMessage = -1;
			RecvStream >> PacketMessage;


		}
		catch (const std::string& Exception) {
			std::cout << Exception << std::endl;
		}

		Recv(EventSocket);
	}
}