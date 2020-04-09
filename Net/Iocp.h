#pragma once
#include "SocketAddress.h"
#include "Config.h"
#include <thread>
#include <vector>

namespace IOCP {
	class CIOCP;
}

namespace TcpSocket {
	class CTcpSocket;
}

namespace WorkerThread {
	void ProcessingQCP(class IOCP::CIOCP* const iocp);
}

namespace IOCP {
	class CIOCP {
		friend static void WorkerThread::ProcessingQCP(CIOCP* const iocp);
	public:
		CIOCP(std::shared_ptr<const CConfig> config);
		~CIOCP();

	public:
		bool Initialize();
		void Run();

	private:
		const size_t mMaxThreadCount;
		const size_t mMaxClientCount;
		const IPEndPoint::CIPEndPoint mServerEndPoint;

	private:
		HANDLE mhIOCP;
		std::vector<std::thread> mWorkerThreads;

	private:
		std::unique_ptr<TcpSocket::CTcpSocket> mListenSocket;
		std::vector<std::unique_ptr<TcpSocket::CTcpSocket>> mClients;

	};

	static WSADATA gWinSockData;
	static bool RegisterIOCompletionPort(HANDLE hIOCP, class TcpSocket::CTcpSocket* const socket);
}