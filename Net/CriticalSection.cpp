#include "CriticalSection.h"

namespace CriticalSection {
	CCriticalSection::CCriticalSection() noexcept {
		InitializeCriticalSection(&mCriticalSection);
	}

	CCriticalSection::~CCriticalSection() noexcept {
		DeleteCriticalSection(&mCriticalSection);
	}

	void CCriticalSection::Enter() noexcept {
		EnterCriticalSection(&mCriticalSection);
	}

	void CCriticalSection::Leave() noexcept {
		LeaveCriticalSection(&mCriticalSection);
	}

}