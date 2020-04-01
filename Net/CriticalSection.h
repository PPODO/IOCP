#pragma once
#include <Windows.h>

namespace CriticalSection {
	class CCriticalSection {
	public:
		explicit CCriticalSection() noexcept;
		~CCriticalSection() noexcept;

	public:
		void Enter() noexcept;
		void Leave() noexcept;

	private:
		CRITICAL_SECTION mCriticalSection;

	};

}