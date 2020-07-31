#pragma once
#include "utils.h"

enum class EOverlappedType {
	EOT_Default,
	EOT_Accept,
	EOT_Receive,
	EOT_Send,
	EOT_Disconnect
};

struct Overlapped_Base {
public:
	WSAOVERLAPPED mOverlapped;
	EOverlappedType mOverlappedType;
	WSABUF mWSABuffer;

private:
	inline void InitWSA() {
		ZeroMemory(&mOverlapped, sizeof(WSAOVERLAPPED)); 
		ZeroMemory(&mWSABuffer, sizeof(WSABUF));
	}

public:
	Overlapped_Base() : mOverlappedType(EOverlappedType::EOT_Default) { InitWSA(); };
	Overlapped_Base(EOverlappedType overlappedType) : mOverlappedType(overlappedType) { InitWSA(); };

};

class Overlapped_Accpet : public Overlapped_Base {
public:
	DWORD mReceiveBytes;
	void* clientSocket;

private:
	char mTempCh;

public:
	Overlapped_Accpet(void* const clientSocket) : Overlapped_Base(EOverlappedType::EOT_Accept), mReceiveBytes(0), mTempCh(0), clientSocket(clientSocket) { mWSABuffer.buf = &mTempCh; mWSABuffer.len = 1; };

};

class Overlapped_Receive : public Overlapped_Base {
public:
	DWORD mReceiveBytes;

private:
	char mBuffer[1024];

public:
	Overlapped_Receive() : Overlapped_Base(EOverlappedType::EOT_Receive), mReceiveBytes(0) {
		mWSABuffer.buf = mBuffer;
		mWSABuffer.len = 1024;
		ZeroMemory(mBuffer, 1024);
	};

};

class Overlapped_Send : public Overlapped_Base {
public:


public:
	Overlapped_Send() : Overlapped_Base(EOverlappedType::EOT_Send) {};

};

class Overlapped_Disconnect : public Overlapped_Base {
public:
	Overlapped_Disconnect() : Overlapped_Base(EOverlappedType::EOT_Disconnect) {};

};

static void DeleteOverlapped(Overlapped_Base* overlapped) {
	switch (overlapped->mOverlappedType) {
	case EOverlappedType::EOT_Accept:
		delete reinterpret_cast<Overlapped_Accpet*>(overlapped);
		break;
	case EOverlappedType::EOT_Disconnect:
		delete reinterpret_cast<Overlapped_Disconnect*>(overlapped);
		break;
	case EOverlappedType::EOT_Receive:
		delete reinterpret_cast<Overlapped_Receive*>(overlapped);
		break;
	case EOverlappedType::EOT_Send:
		delete reinterpret_cast<Overlapped_Send*>(overlapped);
		break;
	}
}