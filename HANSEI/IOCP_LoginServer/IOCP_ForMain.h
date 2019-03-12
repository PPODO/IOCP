#pragma once
#include "../IOCP_Server/IOCP_Base.h"
#include "PacketProcess.h"
#include <thread>
#include <vector>
#include <mutex>

class IOCP_ForMain : public IOCP_Base {
private:
	std::mutex m_Lock;
	std::vector<std::thread> m_WorkerThread;

private:
	class PacketProcess* m_PacketProcessor;

protected:
	virtual bool CreateWorkerThread() override;
	virtual void ProcessWorkerThread() override;

public:
	IOCP_ForMain();
	virtual ~IOCP_ForMain() override;

public:
	virtual void StartIOCP() override;

};