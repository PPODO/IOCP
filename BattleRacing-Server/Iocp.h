#pragma once
#include "TcpSocket.h"
#include "SocketAddress.h"
#include "overlapped.h"
#include <thread>
#include <vector>

class Iocp {
public:
	explicit Iocp();
	~Iocp();

	bool Initialize();

private:
	bool InitializeWorkerThread();
	bool InitializeSockets();

	bool WorkerThread();

	bool RegisterIoCompletionPort(const TcpSocket& clientSocket);

private:
	void OnAccept(Overlapped_Accpet* const acceptOverlapped);

public:
	static const size_t Max_WorkerThread_Count;
	static const size_t Max_Client_Socket_Count;

private:
	HANDLE mhIOCP;

	TcpSocket mListenSocket;
	std::vector<TcpSocket*> mClients;
	
	std::vector<std::thread> mWorkerThreads;

};