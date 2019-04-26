#include "IOCP.h"
#include "PacketProcessor.h"
#include "GamePlayPacket.h"
#include <iostream>
#include <string>
#include <memory>

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
			if (EventSocket) {
				auto Client = m_Clients.find(EventSocket->m_Socket);
				if (Client != m_Clients.end() && Client->second) {
					delete Client->second;
				}
				std::cout << "Server : Disconnet Socket - " << EventSocket->m_Socket << std::endl;
				m_Clients.erase(EventSocket->m_Socket);
				closesocket(EventSocket->m_Socket);
				delete EventSocket;
			}
			continue;
		}

		auto Client = m_Clients.find(EventSocket->m_Socket);
		if (Client != m_Clients.cend()) {
			try {
				// Packet Combination
				char* PacketBuffer = Client->second->m_PacketBuffer;
				if (PacketBuffer) {
					// Process Recv Packet
					if (Client->second->GetPacketSize() > Client->second->m_PrevPacketSize) {
						memcpy(PacketBuffer + Client->second->m_PrevPacketSize, EventSocket->m_DataBuffer.buf, RecvBytes);
						Client->second->m_PrevPacketSize += RecvBytes;
					}
					if (Client->second->GetPacketSize() <= Client->second->m_PrevPacketSize) {
						GAMEPACKET* Packet = (GAMEPACKET*)PacketBuffer;
						if (Packet) {
							if (m_Processor && static_cast<size_t>(Packet->m_PacketType) >= 0 && Packet->m_PacketType < EPACKETTYPE::EPT_COUNT && static_cast<size_t>(Packet->m_MessageType) >= 0 && Packet->m_MessageType < EPACKETMESSAGETYPE::EPMT_COUNT && m_Processor->operator[](Packet->m_MessageType)) {
								m_Processor->operator[](Packet->m_MessageType)(EventSocket, Packet);
							}
							else {
								throw "Unknown Packet!";
							}
						}
						ClearClientPacketBuffer(Client);
					}
				}
			}
			catch (const std::string& Exception) {
				std::cout << Exception << std::endl;
			}
		}

		Recv(EventSocket);
	}
}