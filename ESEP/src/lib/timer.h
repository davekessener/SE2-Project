#ifndef ESEP_LIB_TIMER_H
#define ESEP_LIB_TIMER_H

#include <functional>
#include <atomic>
#include <mutex>
#include <chrono>
#include <stdexcept>
#include <map>

#include "lib/utils.h"
#include "lib/singleton.h"
#include "lib/thread.h"
#include "qnx/channel.h"

namespace esep
{
	namespace lib
	{
		namespace timer
		{
			/**
			 * Timer class
			 *
			 * The Timer Singleton is accessed through lib::Timer::instance()
			 *
			 * Allows users to register callbacks (signatur: void(void) ) to
			 * be called at a later time in a dedicated Timer thread.
			 * User can specify offset (when the callback will be called)
			 * and period (in what intervall the callback is called) in ms.
			 *
			 * The registerCallback() function return a TimerManager object.
			 * This object represents the registered callback and can be passed
			 * to the unregisterCallback() function to remove the callback from
			 * the internal callback storage.
			 *
			 * A TimerManager object also automatically unregisters the associated
			 * callback when its destructor is called.
			 *
			 * The Timer thread operates at a frequency of 1kHz. This means that
			 * any single callback can be called at most every 1ms. Timer does not
			 * spawn a new thread for callback execution. This means that callbacks
			 * should prioritize execution speed and avoid blocking/suspending the
			 * current thread at all cost. If the Timer is not able to call all
			 * scheduled timer in any given update tick it will terminate the
			 * program by throwing an instance of TimerOverflowException in the
			 * Timer thread. // TODO
			 *
			 * The Timer class is thread safe and supports registering as well as
			 * unregistering callbacks from inside executing callbacks.
			 */
			class Impl
			{
				private:
				typedef uint32_t id_t;
				typedef std::unique_lock<std::mutex> lock_t;

				static constexpr id_t INVALID_TIMER_ID = 0;


				public:
				typedef std::function<void(void)> callback_t;

				MXT_DEFINE_E(TimerOverflowException);

				class TimerManager
				{
					public:
						TimerManager( ) : mID(INVALID_TIMER_ID) { }
						TimerManager(TimerManager&&);
						~TimerManager( );
						TimerManager& operator=(TimerManager&&);
						void swap(TimerManager&) noexcept;
						void reset( ) { TimerManager tm; swap(tm); }
					private:
						TimerManager(id_t id) : mID(id) { }

					private:
						id_t mID;

					private:
						TimerManager(const TimerManager&) = delete;
						TimerManager& operator=(const TimerManager&) = delete;

						friend class Impl;
				};


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


				public:
					Impl( );
					~Impl( );

					TimerManager registerCallback(callback_t, uint, uint = 0);
					void unregisterCallback(const TimerManager&);

					// Elapsed time in ms since last reset()/Timer construction
					uint64_t elapsed( );
					// suspends calling thread for a minimum of t ms
					static void sleep(uint t) { std::this_thread::sleep_for(std::chrono::milliseconds(t)); }

					void reset( ) { mSystemStart = std::chrono::system_clock::now(); }
				private:
					void update( );

				private:
					std::chrono::time_point<std::chrono::system_clock> mSystemStart;
					qnx::Connection mConnection;
					lib::Thread mTimerThread;
					std::atomic<bool> mRunning;
					std::map<id_t, Timer> mTimers;
					std::mutex mMutex;
					id_t mNextID;
			};
		}

		typedef SingletonHolder<timer::Impl> Timer;
	}
}

#endif
