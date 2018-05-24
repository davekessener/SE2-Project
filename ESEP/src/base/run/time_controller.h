/*
 * timer.h
 *
 *  Created on: 24.05.2018
 *      Author: acg157
 */

#ifndef SRC_BASE_RUN_TIMER_H
#define SRC_BASE_RUN_TIMER_H

#include <functional>

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
				typedef std::function<void(TimerEvent)> callback_fn;
				typedef std::map<id_t, timer::Manager> timerMap_t;

				public:
					TimeCtrl(callback_fn);
					void setTimer(id_t itemID, TimerEvent, uint, uint = 0);
					void deleteTimer(id_t itemID);
					void pauseAllTimer();
					void resumeAllTimer();

				private:
					timerMap_t mTimer;
					callback_fn mCallback;
			};
		}
	}
}



#endif /* SRC_BASE_RUN_TIMER_H */
