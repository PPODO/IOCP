#pragma once
#include "MultiThreadSynchronize.hpp"
#include <boost/optional.hpp>
#include <memory>

namespace Queue {
	template<typename type, size_t max_queue_length = 512>
	class CCircularQueue : MultiThreadSynchronize::CMultiThreadSync<CCircularQueue<type, max_queue_length>> {
	public:
		explicit CCircularQueue() noexcept : mHeadIndex(), mTailIndex() {
			std::memset(mQueue, 0, sizeof(type) * max_queue_length);
		}

		~CCircularQueue() noexcept {

		}

	public:
		void push_back(const type& data) {
			if ((mTailIndex + 1) % max_queue_length == mHeadIndex) {

				return;
			}
			mTailIndex = (mTailIndex + 1) % max_queue_length;
			mQueue[mTailIndex] = data;
		}

		boost::optional<type> pop_front() {
			if (empty()) {
				return {};
			}
			mHeadIndex = (mHeadIndex + 1) % max_queue_length;
			return mQueue[mHeadIndex];
		}

		__forceinline bool empty() noexcept {
			if (mHeadIndex == mTailIndex) {
				return true;
			}
			return false;
		}

	private:
		type mQueue[max_queue_length];
		size_t mHeadIndex, mTailIndex;

	};

}