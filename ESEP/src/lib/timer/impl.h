#ifndef ESEP_TIMER_IMPL_H
#define ESEP_TIMER_IMPL_H

#include <memory>
#include <mutex>
#include <forward_list>
#include <functional>
#include <map>

#include "lib/timer/types.h"
#include "lib/timer/manager.h"
#include "lib/timer/base.h"

#include "lib/utils.h"
#include "lib/thread.h"

#include "qnx/channel.h"

namespace esep
{
	namespace timer
	{
		class Impl
		{
			typedef std::unique_ptr<Base> Timer_ptr;
			typedef std::unique_lock<std::mutex> lock_t;

			public:
				Impl( );
				~Impl( );

				Manager registerCallback(callback_fn, uint, uint = 0);
				Manager registerAsync(callback_fn, uint, uint = 0);
				void unregisterCallback(const Manager&);

				// Elapsed time in ms since last reset()/Timer construction
				uint64_t elapsed( );
				// suspends calling thread for a minimum of t ms
				static void sleep(uint t) { std::this_thread::sleep_for(std::chrono::milliseconds(t)); }

				void reset( );
			private:
				Manager addTimer(Timer_ptr);
				void update( );
				id_t nextID( );

			private:
				time_t mSystemStart;
				qnx::Connection mConnection;
				lib::Thread mTimerThread;
				std::atomic<bool> mRunning;
				std::map<id_t, Timer_ptr> mTimers;
				std::forward_list<Timer_ptr> mQueue;
				id_t mNextID;
				uint mCounter;
				std::mutex mMutex;
		};
	}
}

#endif
