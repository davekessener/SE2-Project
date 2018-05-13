
#include "base/error/recoverable_error.h"
#include "system.h"

namespace esep { namespace base {

IRecoverableError::IRecoverableError(communication::IRecipient *handler)
	: mHandler(handler)
	, SWITCH(System::instance().get<hal::Switch>())
	, LIGHTS(System::instance().get<hal::Lights>())
	, LEDS(System::instance().get<hal::LEDs>())
	, MOTOR(System::instance().get<hal::Motor>())
	, BUTTONS(System::instance().get<hal::Buttons>())
	, mState(State::PENDING)
{

}

void IRecoverableError::enter()
{
	MOTOR.stop();
	SWITCH.close();
	LIGHTS.flash(Light::RED, MXT_1Hz);
	LEDS.turnOn(LED::RESET);
	mState = State::PENDING;
}

void IRecoverableError::leave()
{
	LEDS.turnOff(LED::RESET);
	LIGHTS.turnOff(Light::RED);
}

}}
