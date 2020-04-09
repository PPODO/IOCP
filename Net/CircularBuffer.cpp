#include "CircularBuffer.hpp"

namespace Buffer {
	CCircularBuffer::CCircularBuffer(size_t buffer_size) noexcept :
		mBufferSize(buffer_size), mBuffer(new char[buffer_size]), 
		mBufferEnd(mBuffer.get() + (buffer_size - 1)), mBufferFront(mBuffer.get()), 
		mBufferRear(mBuffer.get()), mTotalBytes() 
	{
		std::memset(mBuffer.get(), 0, sizeof(char) * buffer_size);
	}

	CCircularBuffer::~CCircularBuffer() {}

	void CCircularBuffer::Push(const char* const buffer) noexcept {
		using namespace MultiThreadSynchronize;
		CSynchronizeType<CCircularBuffer> Sync(this);

		size_t length = std::strlen(buffer);
		if (length > mBufferSize - 1) {
			std::cout << "buffer length must be less than " << mBufferSize - 1 << "bytes!";
			return;
		}

		if (length <= mBufferEnd - mBufferRear - 1) {
			std::memcpy(mBufferRear, buffer, length);
			mBufferRear += length;
		}
		else {
			int remainBytes = mBufferEnd - mBufferRear;
			std::memcpy(mBufferRear, buffer, remainBytes);
			mBufferRear = mBuffer.get();
			std::memcpy(mBufferRear, buffer + remainBytes, length - remainBytes);
			mBufferRear += length - remainBytes;
		}
		mTotalBytes += length;
	}

	void CCircularBuffer::Pop(char* const buffer, size_t length) noexcept {
		using namespace MultiThreadSynchronize;
		CSynchronizeType<CCircularBuffer> Sync(this);

		if (length > mBufferSize - 1) {
			std::cout << "buffer length must be less than " << mBufferSize - 1 << "bytes!";
			return;
		}

		if (length <= mBufferEnd - mBufferFront - 1) {
			std::memcpy(buffer, mBufferFront, length);
			mBufferFront += length;
		}
		else {
			int remainBytes = mBufferEnd - mBufferFront;
			std::memcpy(buffer, mBufferFront, remainBytes);
			mBufferFront = mBuffer.get();
			std::memcpy(buffer + remainBytes, mBufferFront, length - remainBytes);
			mBufferFront += length - remainBytes;
		}
		mTotalBytes -= (length >= mTotalBytes ? mTotalBytes : length);
	}

}