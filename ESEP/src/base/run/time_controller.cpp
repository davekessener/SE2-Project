#include "base/run/time_controller.h"

#include "lib/logger.h"

namespace esep { namespace base { namespace run {

TimeCtrl::TimeCtrl(callback_fn c)
	: mCallback(c),
	  mPaused(false)
{
}

void TimeCtrl::pauseAllTimer()
{
	mPaused = true;
	for(auto& q : mTimer)
	{
		for(auto& t : q)
		{
			lib::Timer::instance().pauseCallback(t);
		}
	}
}

void TimeCtrl::resumeAllTimer()
{
	mPaused = false;
	for(auto& q : mTimer)
	{
		for(auto& t : q)
		{
			lib::Timer::instance().resumeCallback(t);
		}
	}
}

void TimeCtrl::resumeAllTimerDelayed(uint delay)
{
	mDelayTimer = lib::Timer::instance().registerAsync([this](void)
			{
				resumeAllTimer();
			}, delay);
}

void TimeCtrl::setTimer(State state, TimerEvent e, uint r, uint p)
{
	uint8_t s = static_cast<uint8_t>(state);

	if(s < N_STATES)
	{
		auto f = [this, e, state](void) {
			MXT_LOG_INFO("Timer in ", state, " expired; sending ", e);
			mCallback(e);
		};

		MXT_LOG_INFO("Registering timer for ", state, " on ", e, " in ", r);

		mTimer[s].emplace_back(lib::Timer::instance().registerCallback(f, r, p));

		if(mPaused.load())
		{
			lib::Timer::instance().pauseCallback(mTimer[s].back());
		}
	}
	else
	{
		MXT_THROW_E(TimerAccessException, lib::stringify("There is no state with number ", s));
	}
}

void TimeCtrl::deleteTimer(State state)
{
	uint8_t s = static_cast<uint8_t>(state);

	if(!mTimer[s].empty() && s < N_STATES)
	{
		MXT_LOG_INFO("Deleting timer for ", state);

		mTimer[s].pop_front();
	}
	else
	{
		MXT_THROW_E(TimerAccessException, lib::stringify("There is no timer to delete in state ", s));
	}
}

}}}


