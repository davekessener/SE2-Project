#include "ready_manager.h"

#include "hal.h"

namespace esep{ namespace base{

ReadyManager::ReadyManager(communication::IRecipient *basehandler)
	:mBaseHandler(basehandler)
{
}

void ReadyManager::handle(hal::HAL::Event event)
{
	typedef communication::Message Message;
	typedef communication::Packet Packet;
	typedef Packet::Location Location;
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;

	if(event == Event::BTN_START && !HAL_BUTTONS.isPressed(Button::START))
	{
		mBaseHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::RUN));
	}
}

}}
