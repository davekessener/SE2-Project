#include "base/error/reset_ack.h"

#include "system.h"

namespace esep { namespace base { namespace error {

typedef hal::LEDs::LED LED;
typedef hal::Buttons::Button Button;

void ResetAck::enter(void)
{
	HAL_LEDS.turnOn(LED::RESET);
}

void ResetAck::handle(Event e)
{
	if(e == Event::BTN_RESET && HAL_BUTTONS.isPressed(Button::RESET))
	{
		acknowledge();

		HAL_LEDS.turnOff(LED::RESET);
	}
}

}}}
