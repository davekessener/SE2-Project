#include "base/error/item_disappeared.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::Buttons::Button Button;

ItemDisappeared::ItemDisappeared(communication::IRecipient * handler)
	: ResetAck(handler)
{
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



