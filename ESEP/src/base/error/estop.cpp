#include "base/error/estop.h"

#include "hal.h"

#include "hal/buttons.h"
#include "hal/leds.h"

namespace esep { namespace base { namespace error {

typedef hal::Buttons::Button Button;
typedef hal::LEDs::LED LED;

Estop::Estop(communication::IRecipient *handler)
	: Recoverable(handler)
	, mPriority(HAL_BUTTONS.isPressed(Button::ESTOP) ? 5 : 4)
{
}

void Estop::enter(void)
{
	Recoverable::enter();

	if(!HAL_BUTTONS.isPressed(Button::ESTOP))
	{
		acknowledge();
		solve();
	}
	else
	{
		HAL_CONSOLE.println("E-Stop was pushed!");
		MXT_LOG_WARN("E-Stop was pushed!");
	}
}

void Estop::handle(Event e)
{
	switch(e)
	{
	case Event::BTN_ESTOP:
		if(!HAL_BUTTONS.isPressed(Button::ESTOP)) solve();
		break;

	case Event::BTN_RESET:
		if(!HAL_BUTTONS.isPressed(Button::ESTOP) && HAL_BUTTONS.isPressed(Button::RESET)) acknowledge();
		break;

	default:
		break;
	}
}

}}}
