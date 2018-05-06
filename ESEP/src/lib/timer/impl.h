#ifndef ESEP_TIMER_IMPL_H
#define ESEP_TIMER_IMPL_H

#include <map>
#include <forward_list>

#include "lib/timer/types.h"
#include "lib/timer/manager.h"

#include "lib/utils.h"
#include "lib/thread.h"

#include "qnx/channel.h"

namespace esep
{
	namespace timer
	{
		class Impl
		{
			public:
				Impl( );
				~Impl( );

				Manager registerCallback(callback_t, uint, uint = 0);
				Manager registerAsync(callback_t, uint, uint = 0);
				void unregisterCallback(const Manager&);

				// Elapsed time in ms since last reset()/Timer construction
				uint64_t elapsed( );
				// suspends calling thread for a minimum of t ms
				static void sleep(uint t) { std::this_thread::sleep_for(std::chrono::milliseconds(t)); }

				void reset( );
			private:
				void update( );

			private:
				time_t mSystemStart;
				qnx::Connection mConnection;
				lib::Thread mTimerThread;
				std::atomic<bool> mRunning;
				std::map<id_t, Timer> mTimers;
				std::mutex mMutex;
				id_t mNextID;
				uint mCounter;
		};
	}
}

#endif
