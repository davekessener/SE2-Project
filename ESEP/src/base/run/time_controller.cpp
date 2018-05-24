#include <base/run/time_controller.h>

namespace esep { namespace base { namespace run {

#define MXT_CAST(t)		static_cast<uint8_t>(t)
#define MXT_P_NR_STATES	14

TimeCtrl::TimeCtrl(callback_fn c)
	: mCallback(c)
{
}

void TimeCtrl::pauseAllTimer()
{
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
	for(auto& q : mTimer)
	{
		for(auto& t : q)
		{
			lib::Timer::instance().resumeCallback(t);
		}
	}
}

void TimeCtrl::setTimer(State state,TimerEvent e, uint r, uint p)
{
	uint8_t s = MXT_CAST(state);
	if(s <= MXT_P_NR_STATES)
	{
		auto f = [this, e](void) { mCallback(e); };
		mTimer[s].push_back(lib::Timer::instance().registerCallback(f, r, p));
	}
	else
	{
		MXT_THROW_E(TimerAccessExsception, "There is no state with number "+s);
	}
}

void TimeCtrl::deleteTimer(State state)
{
	uint8_t s = MXT_CAST(state);
	if(!mTimer[s].empty() && s <= MXT_P_NR_STATES)
	{
		mTimer[s].pop_front();
	}
	else
	{
		MXT_THROW_E(TimerAccessExsception, "There is no timer to delete in state "+s);
	}
}

}}}


