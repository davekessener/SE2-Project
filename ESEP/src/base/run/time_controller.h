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
#include "data/data_point.h"
#include "data/run_manager_timer_data.h"


namespace esep
{
	namespace base
	{
		namespace run
		{
			class TimeCtrl
			{
				static constexpr size_t N_STATES = 16;
				typedef data::RunManagerTimer::TimerEvent TimerEvent;
				typedef std::function<void(TimerEvent)> callback_fn;
				typedef std::array<std::deque<timer::Manager>, N_STATES> timerArray_t;

				public:
					MXT_DEFINE_E(TimerAccessException);

				public:
					TimeCtrl(callback_fn);
					void setTimer(State, TimerEvent, uint, uint = 0);
					void deleteTimer(State);
					void pauseAllTimer();
					void resumeAllTimer();
					void resumeAllTimerDelayed(uint);

				private:
					timerArray_t mTimer;
					callback_fn mCallback;
					std::atomic<bool> mPaused;
					timer::Manager mDelayTimer;
			};
		}
	}
}



#endif /* SRC_BASE_RUN_TIMER_H */
