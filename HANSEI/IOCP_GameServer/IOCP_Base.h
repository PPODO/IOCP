#pragma once
#include "Socket.h"
#include <sstream>

static const size_t MaxMessageBufferSize = 1024;

typedef struct {
	WSAOVERLAPPED m_Overlapped;
	WSABUF m_DataBuffer;
	SOCKET m_Socket;
	char m_MessageBuffer[MaxMessageBufferSize];
	size_t RecvBytes, SendBytes;
}SOCKETINFO;

class IOCP_Base {
private:
	Socket* m_Server;
	HANDLE m_hIOCP;

protected:
	virtual bool CreateWorkerThread() = 0;
	virtual void ProcessWorkerThread() = 0;

public:
	IOCP_Base();
	virtual ~IOCP_Base();

public:
	bool Init(const int Port);
	bool Send(SOCKETINFO* Info);
	bool Send(SOCKETINFO* Info, std::stringstream&);
	bool Recv(SOCKETINFO* Info);

	virtual void StartIOCP();

protected:
	HANDLE GetIOCPHandle() const { return m_hIOCP; }

};