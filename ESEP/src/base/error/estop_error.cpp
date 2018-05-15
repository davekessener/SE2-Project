
#include <base/error/estop_error.h>

namespace esep { namespace base {

EstopError::EstopError(communication::IRecipient *handler)
	: IRecoverableError(handler)
{

}

void EstopError::handle(Event event)
{
	switch (mState)
	{
		case State::PENDING :
			if (event == Event::BTN_ESTOP && !BUTTONS.isPressed(Button::ESTOP))
			{
				mState = State::PENDING_ACK;
			};
			break;

		case State::PENDING_ACK :
			if (event == Event::BTN_RESET && BUTTONS.isPressed(Button::RESET))
			{
				LEDS.turnOff(LED::RESET);
				auto msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::ERROR_FIXED);
				mHandler->accept(msg);
			};
			break;
		default:
			break;
	}

}

}}
