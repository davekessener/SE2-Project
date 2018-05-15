#ifndef ESEP_LIB_TIMER_H
#define ESEP_LIB_TIMER_H

#include "lib/timer/impl.h"

#include "lib/singleton.h"

namespace esep
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
		 * The registerCallback() function return a Manager object.
		 * This object represents the registered callback and can be passed
		 * to the unregisterCallback() function to remove the callback from
		 * the internal callback storage.
		 *
		 * A Manager object also automatically unregisters the associated
		 * callback when its destructor is called.
		 *
		 * The Timer thread operates at a frequency of 1kHz. This means that
		 * any single callback can be called at most every 1ms. Timer does not
		 * spawn a new thread for callback execution. This means that callbacks
		 * should prioritize execution speed and avoid blocking/suspending the
		 * current thread at all cost.
		 *
		 * The Timer class is thread safe and supports registering as well as
		 * unregistering callbacks from inside executing callbacks.
		 */
	}

	namespace lib
	{
		typedef Singleton<timer::Impl, -2> Timer;
	}
}

#endif
