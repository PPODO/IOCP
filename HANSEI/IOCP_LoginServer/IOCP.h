#pragma once
#include "IOCP_Base.h"
#include <mutex>
#include <thread>
#include <vector>

class IOCP : public IOCP_Base {
private:
	std::mutex m_Lock;
	std::vector<std::thread> m_WorkerThread;

private:
	class PacketProcessor* m_Processor;

public:
	IOCP(const int Port);
	virtual ~IOCP() override;

public:
	virtual void Start() override;

public:
	virtual bool CreateWorkerThread() override;
	virtual void ProcessWorkerThread() override;

};