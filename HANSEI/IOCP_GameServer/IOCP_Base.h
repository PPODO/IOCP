#pragma once
#include "Socket.h"

const size_t MaxMessageBuffer = 1024;

typedef struct {
	WSAOVERLAPPED m_Overlapped;
	WSABUF m_DataBuffer;
	char m_MessageBuffer[MaxMessageBuffer];
	size_t m_SendBytes, m_RecvBytes;
	SOCKET m_Socket;
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

protected:
	virtual bool CreateWorkerThread() = 0;
	virtual void ProcessWorkerThread() = 0;

public:
	bool Recv(SOCKETINFO* Info);
	bool Send(SOCKETINFO* Info, struct GAMEPACKET*& Packet);

public:
	HANDLE GetIOCPHandle() const { return m_hIOCP; }

};