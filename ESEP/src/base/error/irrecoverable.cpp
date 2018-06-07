#include "base/error/irrecoverable.h"

#include "hal.h"

#include "lib/logger.h"

namespace esep { namespace base { namespace error {

typedef hal::Lights::Light Light;

Irrecoverable::Irrecoverable(communication::IRecipient *handler, Message::Error msg)
	: mHandler(handler)
	, mMessage(msg)
{
}

void Irrecoverable::enter()
{
	Base::enter();

	HAL_LIGHTS.flash(Light::RED, ONE_HZ);

	switch(mMessage)
	{
		case Message::Error::SERIAL:
			MXT_LOG_FATAL("Lost serial connection. System restart required!");
			break;

		case Message::Error::CONFIG:
			MXT_LOG_FATAL("Calibration failed. System restart required!");
			break;

		default:
			MXT_LOG_FATAL("Unknown error ", mMessage, ". System restart required!");
			break;
	}
}

void Irrecoverable::leave(void)
{
	MXT_LOG_FATAL("Trying to leave Irrecoverable Error Manager!");

}

}}}
