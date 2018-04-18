#ifndef ESEP_LIB_TIMER_H
#define ESEP_LIB_TIMER_H

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <stdexcept>
#include <map>

#include "lib/utils.h"
#include "lib/singleton.h"
#include "qnx/channel.h"

namespace esep
{
	namespace lib
	{
		namespace timer
		{
			class Impl
			{
				public:
				typedef std::function<bool(void)> callback_t;
				typedef uint32_t id_t;

				struct TimerOverflowException : public std::exception { };

				static constexpr id_t INVALID_TIMER_ID = 0;

				private:
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

				typedef std::unique_lock<std::mutex> lock_t;

				public:
					Impl( );
					~Impl( );
					id_t registerCallback(callback_t, uint, uint = 0);
					void unregisterCallback(id_t);
					uint64_t elapsed( );
					static void sleep(uint t) { std::this_thread::sleep_for(std::chrono::milliseconds(t)); }
					void reset( );
				private:
					void update( );

				private:
					std::chrono::time_point<std::chrono::system_clock> mSystemStart;
					qnx::Connection mConnection;
					std::thread mTimerThread;
					std::atomic<bool> mRunning;
					std::map<id_t, Timer> mTimers;
					std::mutex mMutex;
					bool mUpdating;
					id_t mNextID;
			};
		}

		typedef SingletonHolder<timer::Impl> Timer;
	}
}

#endif
