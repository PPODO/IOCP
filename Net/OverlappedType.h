#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include "MemoryPool.hpp"

namespace Overlapped {
	enum class EOverlappedFlag : uint8_t {
		Accept,
		TryDisconnect,
		Disconnect,
		ReceiveZero,
		Receive,
		Send
	};

	struct Base_Overlapped {
	protected:
		Base_Overlapped(EOverlappedFlag overlappedFlag) noexcept : mFlag(overlappedFlag), mUsedBytes() {
			ZeroMemory(&mOverlapped, sizeof(WSAOVERLAPPED));
		}

	public:
		WSAOVERLAPPED mOverlapped;
		EOverlappedFlag mFlag;
		DWORD mUsedBytes;

	};

	struct Accept_Overlapped : public Base_Overlapped, public MemoryManagement::CMemoryPool<Accept_Overlapped> {
	public:
		Accept_Overlapped(void* const requestedClient) : Base_Overlapped(EOverlappedFlag::Accept), mRequestedClient(requestedClient), mOutBuffer() {};

	public:
		void* const mRequestedClient;
		char mOutBuffer;

	};

	struct ReceiveZero_Overlapped : public Base_Overlapped, public MemoryManagement::CMemoryPool<ReceiveZero_Overlapped> {
	public:
		ReceiveZero_Overlapped(char* const buffer, size_t buffer_length) : Base_Overlapped(EOverlappedFlag::ReceiveZero), mFlag() {
			mWSABuf.buf = buffer;
			mWSABuf.len = buffer_length;
		};

	public:
		WSABUF mWSABuf;
		DWORD mFlag;

	};

	struct Receive_Overlapped : public Base_Overlapped, public MemoryManagement::CMemoryPool<Receive_Overlapped> {
	public:
		Receive_Overlapped(char* const buffer, size_t buffer_length) : Base_Overlapped(EOverlappedFlag::Receive), mFlag() {
			mWSABuf.buf = buffer;
			mWSABuf.len = buffer_length;
		};

	public:
		WSABUF mWSABuf;
		DWORD mFlag;

	};

	struct Send_Overlapped : public Base_Overlapped, public MemoryManagement::CMemoryPool<Send_Overlapped> {
	public:
		Send_Overlapped(const char* const buffer, size_t buffer_length) : Base_Overlapped(EOverlappedFlag::Send) {
			mWSABuf.buf = const_cast<char* const>(buffer);
			mWSABuf.len = buffer_length;
		};

	public:
		WSABUF mWSABuf;

	};
	
	void DeleteIOContext(Base_Overlapped* const overlapped);
}