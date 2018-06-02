#include "base/error/reset_ack.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::LEDs::LED LED;
typedef hal::Buttons::Button Button;

void ResetAck::enter(void)
{
	Recoverable::enter();

	HAL_LEDS.turnOn(LED::RESET);
}

void ResetAck::handle(Event e)
{
	if(e == Event::BTN_RESET && HAL_BUTTONS.isPressed(Button::RESET))
	{
		acknowledge();
	}
}

void ResetAck::acknowledge(void)
{
	Recoverable::acknowledge();

	HAL_LEDS.turnOff(LED::RESET);
}

}}}
