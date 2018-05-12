
#include "base/error/irrecoverable_error.h"
#include "system.h"
#include "lib/logger.h"

namespace esep { namespace base {

IrrecoverableError::IrrecoverableError(communication::IRecipient *handler)
	: mHandler(handler)
	, SWITCH(System::instance().get<hal::Switch>())
	, LIGHTS(System::instance().get<hal::Lights>())
	, MOTOR(System::instance().get<hal::Motor>())
	, mPacket(NULL)
{

}

void IrrecoverableError::enter()
{
	MOTOR.stop();
	SWITCH.close();
	LIGHTS.flash(Light::RED, 1000);

	switch (mPacket->message())
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

void IrrecoverableError::accept(std::shared_ptr<communication::Packet> packet)
{
	mPacket = packet;
}

}}
