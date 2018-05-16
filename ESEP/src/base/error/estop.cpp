#include "base/error/estop.h"

#include "system.h"

#include "hal/buttons.h"
#include "hal/leds.h"

namespace esep { namespace base { namespace error {

typedef hal::Buttons::Button Button;
typedef hal::LEDs::LED LED;

Estop::Estop(communication::IRecipient *handler)
	: ResetAck(handler)
{
}

void Estop::enter(void)
{
	ResetAck::enter();

	if(!HAL_BUTTONS.isPressed(Button::ESTOP))
	{
		acknowledge();
		solve();

		HAL_LEDS.turnOff(LED::RESET);
	}
}

void Estop::handle(Event e)
{
	switch(e)
	{
	case Event::BTN_ESTOP:
		if(!HAL_BUTTONS.isPressed(Button::ESTOP))
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
