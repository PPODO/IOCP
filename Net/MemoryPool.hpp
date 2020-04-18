#pragma once
#include <vector>
#include <memory>
#include "MultiThreadSynchronize.hpp"

namespace MemoryManagement {
	class CChunk {
	public:
		explicit CChunk(size_t typeSize, size_t maxAllocLength) noexcept :
			mPointerAddress(new uint8_t[typeSize * maxAllocLength]),
			mTypeSize(typeSize), mMaxAllocLength(maxAllocLength) 
		{
			mHeadAddress = mPointerAddress.get();
			ChunkSplit();
		};

		explicit CChunk(CChunk&& rhs) noexcept :
			mPointerAddress(std::move(rhs.mPointerAddress)), mHeadAddress(rhs.mHeadAddress),
			mTypeSize(rhs.mTypeSize), mMaxAllocLength(rhs.mMaxAllocLength) {
		};

		~CChunk() { mHeadAddress = nullptr; };

		CChunk& operator=(CChunk&& rhs) noexcept {
			this->mPointerAddress = std::move(rhs.mPointerAddress);
			this->mHeadAddress = rhs.mHeadAddress;
			const_cast<size_t&>(this->mTypeSize) = rhs.mTypeSize;
			const_cast<size_t&>(this->mMaxAllocLength) = rhs.mMaxAllocLength;

			return *this;
		}

	public:
		void* Allocate() noexcept {
			if (!mHeadAddress) {
				return nullptr;
			}

			auto returnPtr = mHeadAddress;
			mHeadAddress = *reinterpret_cast<uint8_t**>(mHeadAddress);

			return returnPtr;
		}

		void Deallocate(void* deletePtr) noexcept {
			*reinterpret_cast<uint8_t**>(deletePtr) = mHeadAddress;
			mHeadAddress = static_cast<uint8_t*>(deletePtr);
		}

		bool Is_has(void* ptr) noexcept {
			if (ptr >= mPointerAddress.get() && ptr <= (mPointerAddress.get() + (mTypeSize * mMaxAllocLength))) {
				return true;
			}
			return false;
		}

	private:
		void ChunkSplit() noexcept {
			uint8_t** currentPtr = reinterpret_cast<uint8_t**>(mHeadAddress);
			uint8_t* nextPtr = mHeadAddress;

			for (size_t i = 0; i < mMaxAllocLength - 1; i++) {
				nextPtr += mTypeSize;
				*currentPtr = nextPtr;
				currentPtr = reinterpret_cast<uint8_t**>(nextPtr);
			}
			*currentPtr = nullptr;
		}

	private:
		const size_t mTypeSize;
		const size_t mMaxAllocLength;

		std::unique_ptr<uint8_t[]> mPointerAddress;
		uint8_t* mHeadAddress;

	};

	class CMemoryManager : public MultiThreadSynchronize::CMultiThreadSync<CMemoryManager> {
	public:
		void* Allocate(size_t typeSize, size_t maxAllocLength) noexcept {
			CThreadSynchronize Sync(this);

			if (mChunkList.size() == 0) {
				return mChunkList.emplace(mChunkList.begin(), typeSize, maxAllocLength)->Allocate();
			}

			for (auto& Chunk : mChunkList) {
				auto Alloc = Chunk.Allocate();
				if (!Alloc) {
					return mChunkList.emplace(mChunkList.begin(), typeSize, maxAllocLength * (mChunkList.size() + 1))->Allocate();
				}
				return Alloc;
			}
		}

		void Deallocate(void* deletePtr) noexcept {
			CThreadSynchronize Sync(this);

			for (auto& Chunk : mChunkList) {
				if (Chunk.Is_has(deletePtr)) {
					Chunk.Deallocate(deletePtr);
					break;
				}
			}
		}

	private:
		std::vector<CChunk> mChunkList;

	};

	template<typename type, size_t max_alloc_length = 128>
	class CMemoryPool {
	public:
		static void* operator new(std::size_t size) {
			static_assert(sizeof(type) > sizeof(uint8_t*), "type must be greater than sizeof(uint8_t*)!");
			return mManager.Allocate(sizeof(type), max_alloc_length);
		}

		static void operator delete(void* const deletePtr) {
			mManager.Deallocate(deletePtr);
		}

	private:
		static CMemoryManager mManager;

	};

	template<typename type, size_t max_alloc_length>
	CMemoryManager CMemoryPool<type, max_alloc_length>::mManager;

}