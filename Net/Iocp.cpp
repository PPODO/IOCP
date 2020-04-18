#include "Iocp.h"
#include "Session.h"
#include "WorkerThread.h"
#include <assert.h>

namespace IOCP {
	CIOCP::CIOCP() :
		mhIOCP(INVALID_HANDLE_VALUE), mListenSocket(), 
		mMaxThreadCount(GetNetConfig().mMaxThreadCount == 0 ? std::thread::hardware_concurrency() * 2 : GetNetConfig().mMaxThreadCount),
		mMaxClientCount(GetNetConfig().mMaxClientCount), mServerEndPoint(GetNetConfig().mIPAddress, GetNetConfig().mPortNumber) 
	{
		assert(WSAStartup(WINSOCK_VERSION, &gWinSockData) != SOCKET_ERROR);

		mhIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, NULL);
		assert(mhIOCP);

		mListenSocket = std::make_unique<CSession>();
		assert(mListenSocket);

		for (size_t i = 0; i < mMaxClientCount; i++) {
			mClients.emplace_back(std::make_unique<CSession>());
		}
	}

	CIOCP::~CIOCP() {
		for (const auto& thread : mWorkerThreads) {
			PostQueuedCompletionStatus(mhIOCP, 0, NULL, NULL);
		}

		for (auto& thread : mWorkerThreads) {
			if (thread.joinable()) {
				thread.join();
			}
		}

		CloseHandle(mhIOCP);
		WSACleanup();
	}

	bool CIOCP::Initialize() {
		if (!mListenSocket->Bind(mServerEndPoint) || 
			!mListenSocket->Listen(SOMAXCONN) || 
			!RegisterIOCompletionPort(mhIOCP, mListenSocket.get())) 
		{
			return false;
		}

		for (auto& Client : mClients) {
			if (!Client->Accept(*mListenSocket) || 
				!RegisterIOCompletionPort(mhIOCP, Client.get())) 
			{
				return false;
			}
		}

		for (unsigned int i = 0; i < mMaxThreadCount; i++) {
			mWorkerThreads.emplace_back(WorkerThread::ProcessingQCP, this);
		}
		return true;
	}

	void CIOCP::Run() {
		while (true) {

		}
	}

	bool RegisterIOCompletionPort(HANDLE hIOCP, CSession* const session) {
		if (!(hIOCP = CreateIoCompletionPort(reinterpret_cast<HANDLE>(session->GetSocketHandle()),
											 hIOCP, reinterpret_cast<ULONG_PTR>(session), 0)))
		{
			if (WSAGetLastError() == WSA_INVALID_PARAMETER) {
				return false;
			}
		}
		return true;
	}
}