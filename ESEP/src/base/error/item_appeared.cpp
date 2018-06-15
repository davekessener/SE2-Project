#include "base/error/item_appeared.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::LightBarriers::LightBarrier LightBarrier;

ItemAppeared::ItemAppeared(communication::IRecipient * handler, Location location)
	: ResetAck(handler)
	, mLocation(location)
{
}

void ItemAppeared::enter(void)
{
	HAL_CONSOLE.println("Error: Item appeared in ", mLocation, "!");
	MXT_LOG_ERROR("Error: Item appeared in ", mLocation, "!");

	ResetAck::enter();
}

void ItemAppeared::handle(Event e)
{
	switch(e)
	{
	case Event::LB_START:
		if(mLocation == Location::LB_START && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			solve();
		}
		break;

	case Event::LB_HEIGHTSENSOR:
		if(mLocation == Location::LB_HEIGHTSENSOR && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			solve();
		}
		break;

	case Event::LB_RAMP:
		if(mLocation == Location::LB_RAMP && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
		{
			solve();
		}
		break;

	case Event::LB_SWITCH:
		if(mLocation == Location::LB_SWITCH && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			solve();
		}
		break;

	case Event::LB_END:
		if(mLocation == Location::LB_END && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			solve();
		}
		break;

	default:
		ResetAck::handle(e);
		break;
	}
}

}}}



