#include "WorkerThread.h"
#include "OverlappedType.h"
#include "PacketHandler.h"
#include "Socket.h"
#include "Session.h"
#include "Iocp.h"

namespace WorkerThread {
	void ProcessingQCP(IOCP::CIOCP* const iocp) {
		using namespace Overlapped;
		HANDLE hIOCP = iocp->GetIOCPHandle();

		while (true) {
			DWORD recvBytes;
			void* completionKey = nullptr;
			Base_Overlapped* lpOverlapped = nullptr;
			auto succeed = GetQueuedCompletionStatus(hIOCP, &recvBytes, reinterpret_cast<PULONG_PTR>(&completionKey), reinterpret_cast<LPOVERLAPPED*>(&lpOverlapped), INFINITE);
			if (!completionKey) {
				break;
			}

			auto clientSession = reinterpret_cast<IOCP::CSession*>(completionKey);
			if (lpOverlapped && clientSession) {
				if (!succeed || (recvBytes <= 0 && lpOverlapped->mFlag != EOverlappedFlag::ReceiveZero)) {
					switch (lpOverlapped->mFlag) {
					case EOverlappedFlag::Accept:
						AcceptClient(static_cast<Accept_Overlapped*>(lpOverlapped)->mRequestedClient);
						break;
					case EOverlappedFlag::Disconnect:

						break;
					default:
						std::cout << "Error";
						break;
					}
				}
				else {
					switch (lpOverlapped->mFlag) {
					case EOverlappedFlag::ReceiveZero:
						clientSession->PostReceive();
						break;
					case EOverlappedFlag::Receive:
						clientSession->ReceiveCompletion(recvBytes);
						clientSession->PreReceive();
						break;
					case EOverlappedFlag::Send:

						break;
					}
				}
				DeleteIOContext(lpOverlapped);
			}
		}
	}

	void AcceptClient(void* const client) {
		auto requestedClient = reinterpret_cast<IOCP::CSession*>(client);
		if (requestedClient) {
			std::cout << "Accept!\n";
			requestedClient->PreReceive();
		}
	}

}
