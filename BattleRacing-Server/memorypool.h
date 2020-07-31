#pragma once
#include <memory>
#include <vector>
#include "threadsync.h"

class chunk {
public:
	explicit chunk(size_t typeSize, size_t allocLength)
		: mTypeSize(typeSize), mAllocLength(allocLength), mPointAddress(new uint8_t[typeSize * allocLength]) {
		mHeadPointer = mPointAddress.get();
		ChunkSplit();
	};

	explicit chunk(const chunk& ref)
		: mTypeSize(ref.mTypeSize), mAllocLength(ref.mAllocLength), mPointAddress(std::move(const_cast<chunk&>(ref).mPointAddress)), mHeadPointer(ref.mHeadPointer) {
	};

	chunk& operator=(const chunk& ref) {
		this->mPointAddress.swap(const_cast<chunk&>(ref).mPointAddress);
		this->mAllocLength = ref.mAllocLength;
		this->mTypeSize = ref.mTypeSize;
		this->mHeadPointer = ref.mHeadPointer;

		return *(this);
	}

public:
	void* Allocate() {
		if (!mHeadPointer)
			return nullptr;

		auto returnPtr = mHeadPointer;
		mHeadPointer = *reinterpret_cast<uint8_t**>(mHeadPointer);

		return returnPtr;
	}

	void Deallocate(void* const pointer) {
		*reinterpret_cast<uint8_t**>(pointer) = mHeadPointer;
		mHeadPointer = static_cast<uint8_t*>(pointer);
	}

	bool Is_Has(void* const pointer) {
		if (pointer >= mPointAddress.get() && pointer <= (mPointAddress.get() + (mTypeSize * mAllocLength)))
			return true;
		return false;
	}

private:
	void ChunkSplit() {
		uint8_t** currentPtr = reinterpret_cast<uint8_t**>(mHeadPointer);
		uint8_t* nextPtr = mHeadPointer;

		for (size_t i = 0; i < mAllocLength - 1; i++) {
			nextPtr += mTypeSize;
			*currentPtr = nextPtr;
			currentPtr = reinterpret_cast<uint8_t**>(nextPtr);
		}
		*currentPtr = nullptr;
	}

private:
	size_t mTypeSize;
	size_t mAllocLength;

	std::unique_ptr<uint8_t[]> mPointAddress{ nullptr };
	uint8_t* mHeadPointer{ nullptr };

};

class memory_manager : public multithreadsync<memory_manager> {
public:
	void* Allocate(size_t typeSize, size_t allocateLength) {
		threadsync sync(this);

		if (!mChunkList.size())
			return mChunkList.emplace(mChunkList.begin(), typeSize, allocateLength)->Allocate();

		for (auto& iterator : mChunkList) {
			if (auto allocatedPointer = iterator.Allocate())
				return allocatedPointer;
			return mChunkList.emplace(mChunkList.begin(), typeSize, allocateLength)->Allocate();
		}
		return nullptr;
	}

	void Deallocate(void* const pointer) {
		threadsync sync(this);

		for (auto& iterator : mChunkList) {
			if (iterator.Is_Has(pointer))
				iterator.Deallocate(pointer);
		}
	}

private:
	std::vector<chunk> mChunkList;

};

template<typename _type, size_t max_alloc_length = 256>
class memory_pool {
public:
	static void* operator new(std::size_t size) {
		return mMemoryManager.Allocate(sizeof(_type), max_alloc_length);
	}

	static void operator delete(void* const pointer) {
		mMemoryManager.Deallocate(pointer);
	}

private:
	static memory_manager mMemoryManager;

};

template<typename _type, size_t max_alloc_length>
memory_manager memory_pool<_type, max_alloc_length>::mMemoryManager;