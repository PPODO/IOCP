#pragma once
#include "MultiThreadSynchronize.hpp"
#include <memory>

namespace Buffer {
	class CCircularBuffer : public MultiThreadSynchronize::CMultiThreadSync<CCircularBuffer> {
	public:
		explicit CCircularBuffer(size_t capacity) noexcept;
		~CCircularBuffer();

	public:
		void Remove(size_t length);
		void Commit(size_t length);
		char* GetWriteOnlyBuffer();
		size_t GetFreeSpaceSize();

	public:
		__forceinline char* GetReadOnlyBuffer() {
			CThreadSynchronize Sync(this);

			if (mBufferASize > 0) {
				return mBufferA;
			}
			return mBufferB;
		}
		__forceinline size_t GetStoredSize() {
			CThreadSynchronize Sync(this);

			return mBufferASize + mBufferBSize;
		}

	private:
		__forceinline size_t GetAFreeSpace() const {
			return (mBufferEnd - mBufferA - mBufferASize);
		}
		__forceinline size_t GetFreeSpaceBeforeA() const {
			return mBufferA - mBuffer.get();
		}
		__forceinline size_t GetBFreeSpace() const {
			return (mBufferA - mBufferB - mBufferBSize);
		}

	private:
		const size_t mCapacity;
		
		std::unique_ptr<char[]> mBuffer;
		char* const mBufferEnd;

		char* mBufferA;
		size_t mBufferASize;

		char* mBufferB;
		size_t mBufferBSize;

	};

}