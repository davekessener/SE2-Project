#ifndef ESEP_TIMER_TYPES_H
#define ESEP_TIMER_TYPES_H

#include <functional>
#include <atomic>
#include <mutex>
#include <chrono>

#include "lib/utils.h"

namespace esep
{
	namespace timer
	{
		typedef uint32_t id_t;
		typedef std::unique_lock<std::mutex> lock_t;
		typedef std::chrono::time_point<std::chrono::system_clock> time_t;
		typedef std::function<void(void)> callback_t;

		constexpr id_t INVALID_TIMER_ID = 0;

		enum class Code : int8_t
		{
			SHUTDOWN,
			EXPIRED
		};

		struct Timer
		{
			id_t id;
			callback_t f;
			uint next, period;

			Timer(id_t id, callback_t f, uint n, uint p)
				: id(id), f(f), next(n), period(p) { }
		};
	}
}

#endif
