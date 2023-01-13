#ifndef __CHRONO__
#define __CHRONO__

#ifdef _MSC_VER
#pragma once
#endif

#include <chrono>

namespace M3D
{
	class Chrono
	{
		using Clock = std::chrono::high_resolution_clock;
		using Duration = std::chrono::duration<float>;

	public:
		void  start() { begin = Clock::now(); }
		void  stop() { end = Clock::now(); }
		float elapsedTime() const { return (std::chrono::duration_cast<Duration>(end - begin)).count(); }

	private:
		Clock::time_point begin;
		Clock::time_point end;
	};
} // namespace M3D

#endif // __CHRONO__