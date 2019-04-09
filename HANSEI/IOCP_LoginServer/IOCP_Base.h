#pragma once
#include "Socket.h"
#include <sstream>

const size_t MaxMessageBuffer = 1024;

typedef struct {
	WSAOVERLAPPED m_Overlapped;
	WSABUF m_DataBuffer;
	char m_MessageBuffer[MaxMessageBuffer];
	SOCKET m_Socket;
	size_t m_RecvBytes, m_SendBytes;
}SOCKETINFO;

class IOCP_Base {
private:
	Socket* m_Socket;
	HANDLE m_hIOCP;

public:
	IOCP_Base(const int Port);
	virtual ~IOCP_Base();

public:
	virtual void Start();
	bool Init();

public:
	virtual bool CreateWorkerThread() = 0;
	virtual void ProcessWorkerThread() = 0;

public:
	bool Recv(SOCKETINFO* SocketInfo);
	bool Send(SOCKETINFO* SocketInfo, std::stringstream& SendStream);

public:
	HANDLE GetIOCPHandle() const { return m_hIOCP; }

};