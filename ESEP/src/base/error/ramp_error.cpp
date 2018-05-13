
#include "base/error/ramp_error.h"
#include "system.h"

namespace esep { namespace base {

RampError::RampError(communication::IRecipient *handler)
	: IRecoverableError(handler)
	, LIGHT_BARRIERS(System::instance().get<hal::LightBarriers>())
{

}

void RampError::handle(Event event)
{
	switch(mState)
	{
		case State::PENDING :
			if (event == Event::LB_RAMP && !LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
			{
				LIGHTS.flash(Light::RED, 500);
				mState = State::GONE;
			}
			else if (event == Event::BTN_RESET && BUTTONS.isPressed(Button::RESET))
			{
				LIGHTS.turnOn(Light::RED);
				mState = State::PENDING_ACK;
			};
			break;

		case State::GONE :
			if (event == Event::BTN_RESET && BUTTONS.isPressed(Button::RESET))
			{
				LEDS.turnOff(LED::RESET);
				auto msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::ERROR_FIXED);
				mHandler->accept(msg);
			}
			else if (event == Event::LB_RAMP && LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
			{
				LIGHTS.flash(Light::RED, 1000);
				mState = State::PENDING;
			};
			break;

		case State::PENDING_ACK :
			if (event == Event::LB_RAMP && !LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
			{
				LEDS.turnOff(LED::RESET);
				auto msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::ERROR_FIXED);
				mHandler->accept(msg);
			};
			break;
	}
}

}}
