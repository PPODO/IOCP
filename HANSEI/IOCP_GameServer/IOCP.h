#pragma once
#include "IOCP_Base.h"
#include "Pawn_Packet.h"
#include <mutex>
#include <thread>
#include <vector>
#include <map>

class IOCP : public IOCP_Base {
private:
	std::mutex m_Lock;
	std::vector<std::thread> m_WorkerThread;

private:


protected:
	virtual bool CreateWorkerThread() override;
	virtual void ProcessWorkerThread() override;

public:
	IOCP();
	virtual ~IOCP() override;

public:
	virtual void StartIOCP() override;

};