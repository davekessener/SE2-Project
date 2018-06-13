#include "base/error/item_disappeared.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::Buttons::Button Button;

ItemDisappeared::ItemDisappeared(communication::IRecipient * handler)
	: ResetAck(handler)
{
}

void ItemDisappeared::enter(void)
{
	HAL_CONSOLE.println("Error: An item disappeared!");
	MXT_LOG_ERROR("Error: An item disappeared!");

	ResetAck::enter();
}

void ItemDisappeared::handle(Event e)
{
	if(e == Event::BTN_RESET && HAL_BUTTONS.isPressed(Button::RESET))
	{
		acknowledge();
		solve();
	}
}

}}}



