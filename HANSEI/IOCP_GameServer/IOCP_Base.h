#pragma once
#include "Socket.h"
#include "GamePlayPacket.h"
#include <map>

const size_t MaxMessageBuffer = 1024;

typedef struct {
	WSAOVERLAPPED m_Overlapped;
	WSABUF m_DataBuffer;
	char m_MessageBuffer[MaxMessageBuffer];
	size_t m_SendBytes, m_RecvBytes;
	SOCKET m_Socket;
}SOCKETINFO;

struct CLIENTPACKETINFORMATION {
private:
	size_t m_PacketSize;

public:
	size_t m_PrevPacketSize;
	char m_PacketBuffer[MaxMessageBuffer];

public:
	CLIENTPACKETINFORMATION(const size_t& PacketSize) : m_PacketSize(PacketSize), m_PrevPacketSize(0), m_PacketBuffer("\0") {};

public:
	size_t GetPacketSize() const { return m_PacketSize; }

};

class IOCP_Base {
private:
	Socket* m_Socket;
	HANDLE m_hIOCP;

protected:
	std::map<SOCKET, CLIENTPACKETINFORMATION*> m_Clients;

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
	bool Send(SOCKETINFO* Info, PACKET* Packet);

public:
	HANDLE GetIOCPHandle() const { return m_hIOCP; }

};