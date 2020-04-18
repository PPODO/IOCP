#pragma once

namespace IOCP {
	class CIOCP;
}

namespace WorkerThread {
	void ProcessingQCP(IOCP::CIOCP* const iocp);
	
	static void AcceptClient(void* const client);
}