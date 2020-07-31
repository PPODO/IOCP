#pragma once
#include <Windows.h>

class critical_section {
public:
	critical_section() { InitializeCriticalSection(&mCrticialSection); };
	~critical_section() { DeleteCriticalSection(&mCrticialSection); };

	inline void Lock() {
		EnterCriticalSection(&mCrticialSection);
	}
	
	inline void Unlock() {
		LeaveCriticalSection(&mCrticialSection);
	}

private:
	CRITICAL_SECTION mCrticialSection;

};