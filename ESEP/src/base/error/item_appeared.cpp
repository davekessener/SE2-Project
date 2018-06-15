#include <memory>

#include "base/error/item_appeared.h"

#include "base/error_manager.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::LightBarriers::LightBarrier LightBarrier;

ItemAppeared::ItemAppeared(communication::IRecipient * handler, Packet_ptr packet)
	: ResetAck(handler)
	, mLocation(nullptr)
{
	for (auto p : *packet)
	{
		if(p->type() == DataType::LOCATION)
		{
			mLocation = std::static_pointer_cast<Location>(p);
		}
	}
	if (mLocation == nullptr) MXT_THROW_EX(ErrorManager::NoLocationInPacket);

}

void ItemAppeared::enter(void)
{
	HAL_CONSOLE.println("Error: Item appeared in ", mLocation->to_s(), "!");
	MXT_LOG_ERROR("Error: Item appeared in ", mLocation->to_s(), "!");

	ResetAck::enter();
}

void ItemAppeared::handle(Event e)
{
	switch(e)
	{
	case Event::LB_START:
		if(mLocation->location() == Location::Type::LB_START && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			solve();
		}
		break;

	case Event::LB_HEIGHTSENSOR:
		if(mLocation->location() == Location::Type::LB_HEIGHTSENSOR && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			solve();
		}
		break;

	case Event::LB_RAMP:
		if(mLocation->location() == Location::Type::LB_RAMP && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
		{
			solve();
		}
		break;

	case Event::LB_SWITCH:
		if(mLocation->location() == Location::Type::LB_SWITCH && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			solve();
		}
		break;

	case Event::LB_END:
		if(mLocation->location() == Location::Type::LB_END && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
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



