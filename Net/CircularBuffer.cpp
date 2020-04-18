#include "CircularBuffer.hpp"

namespace Buffer {
	CCircularBuffer::CCircularBuffer(size_t capacity) noexcept :
		mCapacity(capacity), mBuffer(new char[capacity]),
		mBufferEnd(mBuffer.get() + (capacity - 1)), mBufferA(mBuffer.get()),
		mBufferB(nullptr), mBufferASize(), mBufferBSize()
	{
		std::memset(mBuffer.get(), 0, sizeof(char) * capacity);
	}

	CCircularBuffer::~CCircularBuffer() {}

	void CCircularBuffer::Remove(size_t length) {
		CThreadSynchronize Sync(this);

		if (mBufferASize > 0) {
			size_t removeLen = (mBufferASize < length ? mBufferASize : length);
			mBufferASize -= removeLen;
			mBufferA += removeLen;
			length -= removeLen;
		}

		if (length > 0 && mBufferBSize > 0) {
			size_t removeLen = (mBufferBSize < length ? mBufferBSize : length);
			mBufferBSize -= removeLen;
			mBufferB += removeLen;
			length -= removeLen;
		}

		if (mBufferASize == 0) {
			if (mBufferBSize > 0) {
				memmove(mBuffer.get(), mBufferB, mBufferBSize);

				mBufferA = mBuffer.get();
				mBufferASize = mBufferBSize;
				mBufferB = nullptr;
				mBufferBSize = 0;
			}
			else {
				mBufferB = nullptr;
				mBufferBSize = 0;
				mBufferA = mBuffer.get();
				mBufferASize = 0;
			}
		}
	}

	void CCircularBuffer::Commit(size_t length) {
		CThreadSynchronize Sync(this);

		if (mBufferB) {
			mBufferBSize += length;
		}
		else {
			mBufferASize += length;
		}
	}

	char* CCircularBuffer::GetWriteOnlyBuffer() {
		CThreadSynchronize Sync(this);

		if (mBufferB) {
			return mBufferB + mBufferBSize;
		}
		else {
			return mBufferA + mBufferASize;
		}
		return nullptr;
	}

	size_t CCircularBuffer::GetFreeSpaceSize() {
		CThreadSynchronize Sync(this);

		if (mBufferB) {
			return GetBFreeSpace();
		}
		else {
			if (GetAFreeSpace() < GetFreeSpaceBeforeA()) {
				mBufferB = mBuffer.get();
				return GetBFreeSpace();
			}
			return GetAFreeSpace();
		}
	}

	
}