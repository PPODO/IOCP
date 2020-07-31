#include "Iocp.h"
#include "packetprocessor.h"

const size_t Iocp::Max_WorkerThread_Count = std::thread::hardware_concurrency() * 2;
const size_t Iocp::Max_Client_Socket_Count = 256;

Iocp::Iocp() : mhIOCP(INVALID_HANDLE_VALUE) {
}

Iocp::~Iocp() {
	for (const auto& It : mWorkerThreads) {
		PostQueuedCompletionStatus(mhIOCP, 0, 0, 0);
	}
	for (auto& It : mWorkerThreads) {
		if (It.joinable()) {
			It.join();
		}
	}

	mClients.clear();

	if (mhIOCP != INVALID_HANDLE_VALUE) {
		CloseHandle(mhIOCP);
	}
}

bool Iocp::Initialize() {
	mhIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!mhIOCP || !InitializeSockets() || !InitializeWorkerThread()) {
		return false;
	}
	return true;
}

bool Iocp::InitializeWorkerThread() {
	for (size_t i = 0; i < Max_WorkerThread_Count; i++) {
		mWorkerThreads.push_back(std::thread(&Iocp::WorkerThread, this));
	}
	return true;
}

bool Iocp::InitializeSockets() {
	if (!mListenSocket.Bind(SocketAddress("127.0.0.1", 3550)) || !mListenSocket.Listen() || !RegisterIoCompletionPort(mListenSocket)) {
		return false;
	}

	for (size_t i = 0; i < Max_Client_Socket_Count; i++) {
		TcpSocket* client = new TcpSocket();
		if (!client->Accept(mListenSocket) || !RegisterIoCompletionPort(*client)) {
			std::cout << "Socket Initialize Failure!";
			return false;
		}
		mClients.push_back(client);
	}
	return true;
}

bool Iocp::WorkerThread() {
	while (true) {
		DWORD receiveBytes = 0;
		void* completionKey = 0;
		Overlapped_Base* overlapped = nullptr;
		bool result = GetQueuedCompletionStatus(mhIOCP, &receiveBytes, reinterpret_cast<PULONG_PTR>(&completionKey), reinterpret_cast<LPOVERLAPPED*>(&overlapped), INFINITE);
		if (!completionKey) {
			return false;
		}

		TcpSocket* clientSocket = reinterpret_cast<TcpSocket*>(completionKey);
		if (overlapped && clientSocket) {
			if (!result || receiveBytes <= 0) {
				switch (overlapped->mOverlappedType) {
				case EOverlappedType::EOT_Accept:
					OnAccept(reinterpret_cast<Overlapped_Accpet*>(overlapped));
					break;
				case EOverlappedType::EOT_Disconnect:
					//
					break;
				default:
					clientSocket->TransmitSocket();
					break;
				}
			}
			else {
				switch (overlapped->mOverlappedType) {
				case EOverlappedType::EOT_Receive:
					PacketProcessing(overlapped->mWSABuffer.buf);
					clientSocket->Receive();
					break;
				case EOverlappedType::EOT_Send:
					//
					break;
				}
			}
			DeleteOverlapped(overlapped);
		}
	}
	return true;
}

bool Iocp::RegisterIoCompletionPort(const TcpSocket& clientSocket) {
	if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket.GetSocketHandle()), mhIOCP, reinterpret_cast<ULONG_PTR>(&clientSocket), 0) == NULL) {
		return false;
	}
	return true;
}

void Iocp::OnAccept(Overlapped_Accpet* const acceptOverlapped) {
	if (acceptOverlapped) {
		auto clientSocket = reinterpret_cast<TcpSocket*>(acceptOverlapped->clientSocket);
		if (clientSocket) {

			clientSocket->Receive();
		}
	}
}