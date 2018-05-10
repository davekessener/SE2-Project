#include "lib/timer/manager.h"

#include "lib/timer.h"

namespace esep { namespace timer {

Manager::Manager(Manager&& tm)
	: mID(INVALID_TIMER_ID)
{
	swap(tm);
}

Manager::~Manager(void)
{
	lib::Timer::instance().unregisterCallback(*this);
}

Manager& Manager::operator=(Manager&& tm)
{
	Manager t(INVALID_TIMER_ID);

	tm.swap(t);
	t.swap(*this);

	return *this;
}

void Manager::swap(Manager& tm) noexcept
{
	std::swap(mID, tm.mID);
}

}}
