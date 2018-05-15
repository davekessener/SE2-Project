#include "ready_manager.h"

#include "system.h"

namespace esep{ namespace base{

ReadyManager::ReadyManager(communication::IRecipient *basehandler)
	:mBaseHandler(basehandler)
 	,BUTTONS(System::instance().get<hal::Buttons>())
{
}

void ReadyManager::handle(hal::HAL::Event event)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;

	if(event == Event::BTN_START && BUTTONS.isPressed(Button::START))
	{
		mBaseHandler->accept(std::make_shared<communication::Packet>(communication::Packet::Location::BASE, communication::Packet::Location::MASTER, communication::Packet::Message::SELECT_RUN));
	}
}

}}
