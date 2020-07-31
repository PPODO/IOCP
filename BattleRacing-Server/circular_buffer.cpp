#include "circular_buffer.h"
#include "circular_buffer.h"

void circular_buffer::Remove(size_t length) {
	threadsync sync(this);

	if (mBufferA > 0) {
		size_t removeLength = mBufferASize < length ? mBufferASize : length;
		length -= mBufferASize;
		mBufferA += removeLength;
		mBufferASize -= removeLength;
	}

	if (length > 0) {
		mBufferB += length;
		mBufferBSize -= length;
	}

	if (mBufferA == 0) {
		if (mBufferB > 0) {
			mBufferA = mBufferB;
			mBufferASize = mBufferBSize;
			mBufferB = nullptr;
			mBufferBSize = 0;
		}
		else {
			mBufferA = mBufferStart.get();
			mBufferASize = 0;
			mBufferB = nullptr;
			mBufferBSize = 0;
		}
	}
}

void circular_buffer::Commit(size_t length) {
	threadsync sync(this);

	if (mBufferB)
		mBufferBSize += length;
	else
		mBufferASize += length;
}

char* circular_buffer::GetWriteBuffer() {
	threadsync sync(this);

	if (mBufferB)
		return mBufferB + mBufferBSize;
	return mBufferA + mBufferASize;
}

size_t circular_buffer::GetFreeSpaceSize() {
	threadsync sync(this);

	if (mBufferB)
		return GetBufferBFreeSpace();
	else {
		if (GetBufferAFreeSpace() < GetFreeSapceBeforeBufferA()) {
			mBufferB = mBufferStart.get();
			return GetBufferBFreeSpace();
		}
		return GetBufferAFreeSpace();
	}
}