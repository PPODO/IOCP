#include "../IOCP_Server/Socket.cpp"
#include "../IOCP_Server/IOCP_Base.cpp"
#include "IOCP_ForMain.h"
#include <iostream>
#include <string>
#include <sstream>

IOCP_ForMain::IOCP_ForMain() {
	m_PacketProcessor = new PacketProcess(this);
}

IOCP_ForMain::~IOCP_ForMain() {
	if (m_PacketProcessor) {
		delete m_PacketProcessor;
		m_PacketProcessor = nullptr;
	}

	for (auto& It : m_WorkerThread) {
		It.join();
	}
	m_WorkerThread.clear();
}

void IOCP_ForMain::StartIOCP() {
	IOCP_Base::StartIOCP();

}

bool IOCP_ForMain::CreateWorkerThread() {
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	for (size_t i = 0; i < SystemInfo.dwNumberOfProcessors * 2; i++) {
		m_WorkerThread.push_back(std::thread([this]() {
			ProcessWorkerThread();
		}));
	}
	std::cout << "Create Worker Thread Successful!\n";
	return true;
}

void IOCP_ForMain::ProcessWorkerThread() {
	DWORD RecvBytes = 0, CompletionKey = 0;
	SOCKETINFO* EventSocket = nullptr;

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
			int PacketType = -1;
			RecvStream >> PacketType;

			if (PacketType >= 0 && PacketType < PM_COUNT) {
				m_PacketProcessor->operator[]((PACKETMESSAGE)PacketType)(EventSocket, RecvStream);
			}
			else {
				throw "Nonexistent Packet Type!!";
			}
		}
		catch (std::string& Exception) {
			std::cout << Exception << std::endl;
		}

		Recv(EventSocket);
	}
}