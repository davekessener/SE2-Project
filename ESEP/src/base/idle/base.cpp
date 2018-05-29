#include "base/idle/base.h"

#include "system.h"

namespace esep { namespace base {

void IdleBase::handle(Event e)
{
	typedef hal::Buttons::Button Button;

	switch(e)
	{
	case Event::BTN_START:
		if(HAL_BUTTONS.isPressed(Button::START))
		{
			mPressed = lib::Timer::instance().elapsed();
		}
		else
		{
			uint d = lib::Timer::instance().elapsed() - mPressed;

			if(d >= mMax)
			{
				longPress();
			}
			else if(d >= mMin)
			{
				shortPress();
			}
		}
		break;

	default:
		break;
	}
}

}}
