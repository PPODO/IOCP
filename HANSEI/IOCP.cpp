#include <sstream>
#include "IOCP.h"

DWORD WINAPI WorkerThreadFunction(LPVOID Data) {
	IOCP* Worker = (IOCP*)Data;
	if (Worker) {
		Worker->ProcessWorkerThread();
	}
	return 0;
}

IOCP::IOCP() {

}

void IOCP::StartIOCP() {
	IOCPBase::StartIOCP();

}

bool IOCP::CreateWorkerThread() {
	SYSTEM_INFO SystemInfo;
	unsigned long ThreadID = 0;
	GetSystemInfo(&SystemInfo);
	m_ThreadCount = SystemInfo.dwNumberOfProcessors * 2;
	m_hThread = new HANDLE[m_ThreadCount];

	for (int i = 0; i < m_ThreadCount; i++) {
		if (m_hThread[i]) {
			m_hThread[i] = CreateThread(NULL, 0, WorkerThreadFunction, this, 0, &ThreadID);
		}
		else {
			std::cout << "Create Worker Thread Failure!!\n";
			return false;
		}
	}
	std::cout << "Create Worker Thread Successful!!\n" << "Number Of Worker Thread : " << m_ThreadCount << std::endl;
	return true;
}

void IOCP::ProcessWorkerThread() {
	DWORD RecvBytes = 0, CompletionKey = 0;
	SOCKETINFO* EventSocket;

	while (1) {
		if (GetQueuedCompletionStatus(m_hIOCP, &RecvBytes, (PULONG_PTR)&CompletionKey, (LPOVERLAPPED*)&EventSocket, INFINITE) == 0) {
			ThreadLock.lock();
			if (WSAGetLastError() != ERROR_NETNAME_DELETED) {
				std::cout << "Get Queued Completion Status Error!! - " << WSAGetLastError() << std::endl;
				ThreadLock.unlock();
				return;
			}
			ThreadLock.unlock();
		}

		if (RecvBytes == 0) {
			std::cout << "Server : Disconnet Socket - " << EventSocket->Socket << std::endl;
			if (EventSocket) {
				closesocket(EventSocket->Socket);
				delete EventSocket;
			}
			continue;
		}

		try {
			std::cout << EventSocket->DataBuffer.buf << std::endl;
			ThreadLock.lock();
			strcpy(EventSocket->MessageBuffer, "HI_PPODO!");
			EventSocket->DataBuffer.len = MAX_BUFFER;
			EventSocket->DataBuffer.buf = EventSocket->MessageBuffer;
			ThreadLock.unlock();
			Send(EventSocket);
		}
		catch(const char* Exception) {
			std::cout << "Error!! - " << Exception << std::endl;
		}

		Recv(EventSocket);
	}
}

IOCP::~IOCP() {

}