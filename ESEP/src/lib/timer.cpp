#include <chrono>
#include <thread>

#include "lib/timer.h"

namespace esep { namespace lib { namespace timer {

Impl::Impl(void)
{
	reset();
}

void Impl::reset(void)
{
	mSystemStart = std::chrono::system_clock::now();
}

void Impl::registerCallback(callback_t f, uint o, uint p)
{
	// TODO
}

void Impl::sleep(uint ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint64_t Impl::elapsed(void)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mSystemStart).count();
}

}}}
