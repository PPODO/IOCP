#pragma once
#include "IOCP_Base.h"
#include <thread>
#include <vector>
#include <mutex>

const int MaxSavePacketCount = 50;

class IOCP : public IOCP_Base {
private:
	std::vector<std::thread> m_WorkerThread;
	std::mutex m_GetClientLock;

private:
	class PacketProcessor* m_Processor;

private:
	inline void ClearClientPacketBuffer(std::map<SOCKET, CLIENTPACKETINFORMATION*>::iterator& It);
	inline void DeleteClientFromList(SOCKETINFO*& EventSocket);

protected:
	virtual bool CreateWorkerThread() override;
	virtual void ProcessWorkerThread() override;

public:
	IOCP(const int Port);
	virtual ~IOCP() override;

public:
	virtual void Start() override;

};

inline void IOCP::ClearClientPacketBuffer(std::map<SOCKET, CLIENTPACKETINFORMATION*>::iterator& It) {
	memset(It->second->m_PacketBuffer, 0, It->second->GetPacketSize());
	memcpy(It->second->m_PacketBuffer, It->second->m_PacketBuffer + It->second->GetPacketSize(), (It->second->m_PrevPacketSize - It->second->GetPacketSize()));
	It->second->m_PrevPacketSize -= It->second->GetPacketSize();
}