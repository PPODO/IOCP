#pragma once
#include "CriticalSection.h"
#include <iostream>

namespace MultiThreadSynchronize {
	template<typename type>
	class CMultiThreadSync {
	public:
		class CThreadSynchronize {
		public:
			explicit CThreadSynchronize(type* instance) noexcept : mInstance(instance) {
				mInstance->mSync.Enter();
			}

			~CThreadSynchronize() noexcept {
				mInstance->mSync.Leave();
			}

		private:
			type* const mInstance;
		};

	private:
		CriticalSection::CCriticalSection mSync;

	};

	template<typename type>
	using CSynchronizeType = typename CMultiThreadSync<type>::CThreadSynchronize;
}