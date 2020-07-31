#pragma once
#include "critical_section.h"

template<typename _type>
class multithreadsync {
public:
	class threadsync {
	public:
		explicit threadsync(_type* const instance) : mInstance(instance) {
			mInstance->mCriticalSection.Lock();
		}

		~threadsync() {
			mInstance->mCriticalSection.Unlock();
		}

	private:
		_type* const mInstance;

	};

private:
	critical_section mCriticalSection;

};

template<typename _type>
using threadsafe = typename multithreadsync<_type>::threadsync;