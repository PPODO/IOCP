#pragma once
#include "IOCP_Base.h"
#include <thread>
#include <vector>
#include <mutex>
#include <queue>

class IOCP : public IOCP_Base {
private:
	std::vector<std::thread> m_WorkerThread;
	std::mutex m_Lock;

private:
	class PacketProcessor* m_Processor;

private:


protected:
	virtual bool CreateWorkerThread() override;
	virtual void ProcessWorkerThread() override;

public:
	IOCP(const int Port);
	virtual ~IOCP() override;

public:
	virtual void Start() override;

};