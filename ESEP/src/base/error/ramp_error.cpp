
#include "base/error/ramp_error.h"
#include "system.h"

namespace esep { namespace base {

RampError::RampError(communication::IRecipient *handler)
	: mHandler(handler)
	, SWITCH(System::instance().get<hal::Switch>())
	, LIGHT_BARRIERS(System::instance().get<hal::LightBarriers>())
	, LIGHTS(System::instance().get<hal::Lights>())
	, LEDS(System::instance().get<hal::LEDs>())
	, MOTOR(System::instance().get<hal::Motor>())
{

}

void RampError::enter()
{

}

void RampError::leave()
{

}

void RampError::handle(hal::HAL::Event event)
{

}

}}
