#include "base/error/item_stuck.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::LightBarriers::LightBarrier LightBarrier;

ItemStuck::ItemStuck(communication::IRecipient * handler, Location location)
	: ResetAck(handler)
	, mLocation(location)
{
}

void ItemStuck::handle(Event e)
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



