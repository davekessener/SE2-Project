
#include "base/error/irrecoverable_error.h"
#include "system.h"
#include "lib/logger.h"

namespace esep { namespace base {

IrrecoverableError::IrrecoverableError(communication::IRecipient *handler, Message msg)
	: mHandler(handler)
	, SWITCH(System::instance().get<hal::Switch>())
	, LIGHTS(System::instance().get<hal::Lights>())
	, MOTOR(System::instance().get<hal::Motor>())
	, mMessage(msg)
{

}

void IrrecoverableError::enter()
{
	MOTOR.stop();
	SWITCH.close();
	LIGHTS.flash(Light::RED, 1000);

	switch (mMessage)
	{
		case Message::ERROR_SERIAL:
			MXT_LOG("Fatal Error: Lost serial connection. System restart required!");
			break;
		case Message::CONFIG_FAILED:
			MXT_LOG("Fatal Error: Calibration failed. System restart required!");
			break;
		default:
			MXT_LOG("Fatal Error: System restart required!");
			break;
	}
}

}}
