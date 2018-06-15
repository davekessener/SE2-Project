#include "base/error/ramp.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::Buttons::Button Button;
typedef hal::LightBarriers::LightBarrier LightBarrier;

Ramp::Ramp(communication::IRecipient *handler)
	: ResetAck(handler)
{
}

void Ramp::enter(void)
{
	ResetAck::enter();

	if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
	{
		solve();
	}
	else
	{
		HAL_CONSOLE.println("Ramp full!");
	}
}

void Ramp::handle(Event e)
{
	switch(e)
	{
	case Event::LB_RAMP:
		if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
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
