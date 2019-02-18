#pragma once
#include "IOCPBase.h"

class IOCP : public IOCPBase {
private:


private:
	virtual bool CreateWorkerThread() override;

public:
	IOCP();
	virtual void StartIOCP() override;
	virtual void ProcessWorkerThread() override;

	virtual ~IOCP() override;
};