#include <memory>

#include "base/error/item_stuck.h"

#include "base/error_manager.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::LightBarriers::LightBarrier LightBarrier;

ItemStuck::ItemStuck(communication::IRecipient * handler, Packet_ptr packet)
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

void ItemStuck::enter(void)
{
	HAL_CONSOLE.println("Error: Item stuck in ", mLocation->to_s(), "!");
	MXT_LOG_ERROR("Error: Item stuck in ", mLocation->to_s(), "!");

	ResetAck::enter();

	tryResolve();
}

void ItemStuck::handle(Event e)
{
	switch(e)
	{
	case Event::LB_START:
	case Event::LB_HEIGHTSENSOR:
	case Event::LB_RAMP:
	case Event::LB_SWITCH:
	case Event::LB_END:
		tryResolve();
		break;

	default:
		tryResolve();
		ResetAck::handle(e);
		break;
	}
}

void ItemStuck::tryResolve(void)
{
	switch(mLocation->location())
	{
	case Location::Type::LB_START:
		if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			solve();
		}
		break;

	case Location::Type::LB_HEIGHTSENSOR:
		if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			solve();
		}
		break;

	case Location::Type::LB_RAMP:
		if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
		{
			solve();
		}
		break;

	case Location::Type::LB_SWITCH:
		if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			solve();
		}
		break;

	case Location::Type::LB_END:
		if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			solve();
		}
		break;
	}
}

}}}



