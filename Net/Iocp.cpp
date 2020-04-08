#include "Iocp.h"
#include "Socket.h"
#include "OverlappedType.h"
#include <assert.h>

namespace IOCP {
	CIOCP::CIOCP(std::shared_ptr<const CConfig> config) noexcept :
		mhIOCP(INVALID_HANDLE_VALUE), mListenSocket(), 
		mMaxThreadCount(config->mMaxThreadCount), mMaxClientCount(config->mMaxClientCount),
		mServerEndPoint(config->mIPAddress, config->mPortNumber) {

		mhIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, NULL);
		assert(mhIOCP);
	}

	CIOCP::~CIOCP() {
		for (const auto& const thread : mWorkerThreads) {
			PostQueuedCompletionStatus(mhIOCP, 0, NULL, NULL);
		}

		for (auto& thread : mWorkerThreads) {
			if (thread.joinable()) {
				thread.join();
			}
		}

		CloseHandle(mhIOCP);
	}

	bool CIOCP::Initialize() {
		for (unsigned int i = 0; i < mMaxThreadCount; i++) {
			mWorkerThreads.emplace_back(WorkerThread, this);
		}


		return false;
	}

	void WorkerThread(CIOCP* const iocp) {
		using namespace Overlapped;
		HANDLE hIOCP = iocp->mhIOCP;
		
		while (true) {
			DWORD recvBytes;
			void* completionKey = nullptr;
			Base_Overlapped* lpOverlapped = nullptr;
			auto succeed = GetQueuedCompletionStatus(hIOCP, &recvBytes, reinterpret_cast<PULONG_PTR>(&completionKey), reinterpret_cast<LPOVERLAPPED*>(&lpOverlapped), INFINITE);
			if (!completionKey) {
				break;
			}

			if (lpOverlapped) {
				if (!succeed || recvBytes <= 0) {
					switch (lpOverlapped->mFlag) {
					case EOverlappedFlag::Accept:

						break;
					case EOverlappedFlag::Disconnect:

						break;
					default:

						break;
					}
				}

				switch (lpOverlapped->mFlag) {
				case EOverlappedFlag::Receive:

					break;
				case EOverlappedFlag::Send:

					break;
				}
			}
		}
	}
}