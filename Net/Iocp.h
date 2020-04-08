#pragma once
#include "SocketAddress.h"
#include "Config.h"
#include <thread>
#include <vector>

namespace TcpSocket {
	class CTcpSocket;
}

namespace IOCP {
	class CIOCP {
		friend static void WorkerThread(CIOCP* const iocp);
	public:
		CIOCP(std::shared_ptr<const CConfig> config) noexcept;
		~CIOCP();

	public:
		bool Initialize();

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

	static void WorkerThread(CIOCP* const iocp);
}