#include "base/error/analyse.h"

#include "data/message_data.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

namespace
{
	typedef hal::LightBarriers::LightBarrier LightBarrier;
}

Analyse::Analyse(IRecipient *c, Packet_ptr p)
	: ResetAck(c)
{
	for(const auto& d : *p)
	{
		if(d->type() == data::DataPoint::Type::MESSAGE)
		{
			mMessage = static_cast<data::Message&>(*d).message();
		}
	}
}

void Analyse::enter(void)
{
	ResetAck::enter();
}

void Analyse::handle(Event e)
{
	switch(e)
	{
	case Event::LB_SWITCH:
		if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			solve();
		}
		break;

	default:
		break;
	}

	ResetAck::handle(e);
}

}}}
