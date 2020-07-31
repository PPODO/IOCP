#pragma once
#include <memory>
#include "threadsync.h"

class circular_buffer : public multithreadsync<circular_buffer> {
public:
	circular_buffer(size_t capacity)
		: mCapacity(capacity), mBufferStart(new char[capacity]), mBufferEnd(mBufferStart.get() + capacity)
		, mBufferA(), mBufferASize(), mBufferB(), mBufferBSize() {
		std::memset(mBufferStart.get(), 0, capacity);
	};

public:
	void Remove(size_t length);
	void Commit(size_t length);
	char* GetWriteBuffer();
	size_t GetFreeSpaceSize();

	inline char* GetReadBuffer() {
		if (mBufferB)
			return mBufferB;
		return mBufferA;
	}

	inline size_t GetStoredSize() {
		threadsync sync(this);

		return mBufferASize + mBufferBSize;
	}

private:
	inline size_t GetBufferAFreeSpace() {
		return mBufferEnd - mBufferA - mBufferASize;
	}

	inline size_t GetBufferBFreeSpace() {
		return mBufferA - mBufferB - mBufferBSize;
	}

	inline size_t GetFreeSapceBeforeBufferA() {
		return mBufferA - mBufferStart.get();
	}

private:
	std::unique_ptr<char[]> mBufferStart;
	char* mBufferEnd;

	char* mBufferA;
	size_t mBufferASize;

	char* mBufferB;
	size_t mBufferBSize;

	size_t mCapacity;
};