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
	HAL_CONSOLE.println("Error: Ramp is full!");
	MXT_LOG_ERROR("Error: Ramp is full!");

	ResetAck::enter();
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
