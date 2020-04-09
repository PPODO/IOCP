#include "WorkerThread.h"
#include "OverlappedType.h"
#include "Socket.h"
#include "Iocp.h"

namespace WorkerThread {
	void ProcessingQCP(IOCP::CIOCP* const iocp) {
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

			auto clientSocket = reinterpret_cast<TcpSocket::CTcpSocket*>(completionKey);
			if (lpOverlapped && clientSocket) {
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
						std::cout << "22\n";
						clientSocket->PostReceive();
						break;
					case EOverlappedFlag::Receive:

						std::cout << "HI\n";

						clientSocket->PreReceive();
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
		auto requestedClient = reinterpret_cast<TcpSocket::CTcpSocket*>(client);
		if (requestedClient) {
			std::cout << "Accept!\n";
			requestedClient->PreReceive();
		}
	}

}
