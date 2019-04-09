#include "IOCP.h"
#include "GamePlayPacket.h"
#include "PacketProcessor.h"
#include <iostream>
#include <string>

IOCP::IOCP(const int Port) : IOCP_Base(Port), m_Processor(nullptr) {
	m_Processor = new PacketProcessor(this);
}

IOCP::~IOCP() {
	for (size_t i = 0; i < m_WorkerThread.size(); i++) {
		m_WorkerThread[i].join();
	}
	m_WorkerThread.clear();

	if (m_Processor) {
		delete m_Processor;
		m_Processor = nullptr;
	}
}

void IOCP::Start() {
	IOCP_Base::Start();
}

bool IOCP::CreateWorkerThread() {
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

void IOCP::ProcessWorkerThread() {
	DWORD CompletionKey = 0, RecvBytes = 0;
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
			GAMEPACKET* Packet = (GAMEPACKET*)EventSocket->m_DataBuffer.buf;

			if (m_Processor && Packet && static_cast<size_t>(Packet->m_MessageType) >= 0 && Packet->m_MessageType < EPACKETMESSAGETYPE::EPMT_COUNT && m_Processor->operator[](static_cast<size_t>(Packet->m_MessageType))) {
				m_Processor->operator[](static_cast<size_t>(Packet->m_MessageType))(EventSocket, Packet);
			}
			else {
				throw "Unknown Packet!";
			}
		}
		catch (const std::string& Exception) {
			std::cout << Exception << std::endl;
		}
	}
}