#pragma once
#include <mutex>
#include "Socket.h"

#define MAX_BUFFER 1024

typedef struct SOCKETINFO {
	WSAOVERLAPPED Overlapped;
	WSABUF DataBuffer;
	SOCKET Socket;
	char MessageBuffer[MAX_BUFFER];
	int RecvBytes, SendBytes;
}SOCKETINFO;

class IOCPBase {
protected:
	std::mutex ThreadLock;
	Socket m_Server;
	HANDLE m_hIOCP;
	HANDLE *m_hThread;
	int m_ThreadCount;

protected:
	virtual bool CreateWorkerThread() = 0;
	virtual void ProcessWorkerThread() = 0;

public:
	IOCPBase();
	bool Init();
	virtual void StartIOCP();

	void Send(SOCKETINFO* SocketInfo);
	void Recv(SOCKETINFO* SocketInfo);

	virtual ~IOCPBase();
};