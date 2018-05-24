#include <base/run/time_controller.h>

namespace esep { namespace base { namespace run {

TimeCtrl::TimeCtrl(callback_fn c)
	: mCallback(c)
{
}

void TimeCtrl::pauseAllTimer()
{
	for(auto& t : mTimer)
	{
		lib::Timer::instance().pauseCallback(t.second);
	}
}

void TimeCtrl::resumeAllTimer()
{
	for(auto& t : mTimer)
	{
		lib::Timer::instance().resumeCallback(t.second);
	}
}

void TimeCtrl::setTimer(id_t itemID, TimerEvent e, uint r, uint p)
{
	auto f = [this, e](void) { mCallback(e); };
	mTimer.emplace(std::make_pair(itemID, lib::Timer::instance().registerCallback(f, r, p)));
}

void TimeCtrl::deleteTimer(id_t itemID)
{
	mTimer.erase(itemID);
}

}}}


