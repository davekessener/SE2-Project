#ifndef ESEP_LIB_TIMER_H
#define ESEP_LIB_TIMER_H

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

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

				struct TimerOverflowException : public std::exception { };

				private:
				enum class Code : int8_t
				{
					SHUTDOWN,
					EXPIRED
				};

				struct Timer
				{
					callback_t f;
					uint next, period;

					Timer(callback_t f, uint n, uint p) : f(f), next(n), period(p) { }
				};

				typedef std::unique_lock<std::mutex> lock_t;

				public:
					Impl( );
					~Impl( );
					void registerCallback(callback_t, uint, uint = 0);
					uint64_t elapsed( );
					void sleep(uint);
					void reset( );
				private:
					void update( );

				private:
					std::chrono::time_point<std::chrono::system_clock> mSystemStart;
					qnx::Connection mConnection;
					std::thread mTimerThread;
					std::atomic<bool> mRunning;
					std::vector<Timer> mTimers;
					std::mutex mMutex;
					bool mUpdating;
			};
		}

		typedef SingletonHolder<timer::Impl> Timer;
	}
}

#endif
