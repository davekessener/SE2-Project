/*
 * timer.h
 *
 *  Created on: 24.05.2018
 *      Author: acg157
 */

#ifndef SRC_BASE_RUN_TIMER_H
#define SRC_BASE_RUN_TIMER_H

#include <functional>
#include <deque>
#include <array>

#include "lib/timer.h"
#include "lib/timer/types.h"
#include "lib/timer/manager.h"
#include "types.h"


namespace esep
{
	namespace base
	{
		namespace run
		{
			class TimeCtrl
			{
				typedef std::function<void(TimerEvent, State)> callback_fn;
				typedef std::array<std::deque<timer::Manager>, 14> timerArray_t;

				public:
					MXT_DEFINE_E(TimerAccessException);

				public:
					TimeCtrl(callback_fn);
					void setTimer(State, TimerEvent, uint, uint = 0);
					void deleteTimer(State);
					void pauseAllTimer();
					void resumeAllTimer();

				private:
					timerArray_t mTimer;
					callback_fn mCallback;
			};
		}
	}
}



#endif /* SRC_BASE_RUN_TIMER_H */
