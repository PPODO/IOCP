#pragma once
#include "SocketAddress.h"
#include "Config.h"
#include <thread>
#include <vector>

namespace TcpSocket {
	class CTcpSocket;
}

namespace IOCP {
	class CSession;

	class CIOCP {
	public:
		CIOCP();
		~CIOCP();

	public:
		bool Initialize();
		void Run();

	public:
		__forceinline HANDLE GetIOCPHandle() const { return mhIOCP; }

	private:
		const size_t mMaxThreadCount;
		const size_t mMaxClientCount;
		const IPEndPoint::CIPEndPoint mServerEndPoint;

	private:
		HANDLE mhIOCP;
		std::vector<std::thread> mWorkerThreads;

	private:
		std::unique_ptr<CSession> mListenSocket;
		std::vector<std::unique_ptr<CSession>> mClients;

	};

	static WSADATA gWinSockData;
	static bool RegisterIOCompletionPort(HANDLE hIOCP, class CSession* const session);
}