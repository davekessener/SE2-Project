#include "idle_manager.h"

#include "lib/logger.h"

#include "system.h"

namespace esep { namespace base {

IdleManager::IdleManager(communication::IRecipient *basehandler)
	: mBaseHandler(basehandler)
 	, mTime(0)
{
}

void IdleManager::handle(hal::HAL::Event event)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons Buttons;

	if(event == Event::BTN_START)
	{
		if(HAL_BUTTONS.isPressed(Buttons::Button::START))
		{
			mTime = lib::Timer::instance().elapsed();
		}
		else
		{
			auto elap = lib::Timer::instance().elapsed() - mTime;

			if(elap >= T_MAX)
			{
				mBaseHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::CONFIG));
			}
			else if(elap >= T_MIN)
			{
				mBaseHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::RUN));
			}
		}
	};
}

}}
