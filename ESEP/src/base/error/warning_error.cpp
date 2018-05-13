
#include <base/error/warning_error.h>
#include "system.h"

#define MXT_1KHz 1000

namespace esep { namespace base {

WarningError::WarningError(communication::IRecipient *handler)
	: mHandler(handler)
	, SWITCH(System::instance().get<hal::Switch>())
	, LIGHTS(System::instance().get<hal::Lights>())
	, MOTOR(System::instance().get<hal::Motor>())
{

}

void WarningError::enter()
{
	MOTOR.stop();
	SWITCH.close();
	LIGHTS.flash(Light::YELLOW, MXT_1KHz);
}

void WarningError::leave()
{
	LIGHTS.turnOff(Light::YELLOW);
}

}}
