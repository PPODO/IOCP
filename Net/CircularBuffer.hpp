#pragma once
#include "MultiThreadSynchronize.hpp"
#include <memory>

namespace Buffer {
	class CCircularBuffer : public MultiThreadSynchronize::CMultiThreadSync<CCircularBuffer> {
	public:
		explicit CCircularBuffer(size_t buffer_size) noexcept;
		~CCircularBuffer();

	public:
		void Push(const char* const buffer) noexcept;
		void Pop(char* const buffer, size_t length) noexcept;

	public:
		__forceinline size_t GetRemainBytes() noexcept {
			using namespace MultiThreadSynchronize;
			CSynchronizeType<CCircularBuffer> Sync(this);

			return mTotalBytes;
		}

	private:
		const size_t mBufferSize;

		std::unique_ptr<char[]> mBuffer;
		char* const mBufferEnd;
		char* mBufferFront;
		char* mBufferRear;
		size_t mTotalBytes;

	};

}