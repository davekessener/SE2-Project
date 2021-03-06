#ifndef ESEP_TIMER_TYPES_H
#define ESEP_TIMER_TYPES_H

#include <functional>
#include <atomic>
#include <chrono>

#include "lib/utils.h"

namespace esep
{
	namespace timer
	{
		typedef uint32_t id_t;
		typedef std::chrono::time_point<std::chrono::system_clock> time_t;
		typedef std::function<void(void)> callback_fn;

		constexpr id_t INVALID_TIMER_ID = 0;
	}
}

#endif
