#include <base/error/estop.h>
#include <base/error/irrecoverable.h>
#include <base/error/ramp.h>
#include <base/error/warning.h>
#include "base/error_manager.h"

#include "lib/logger.h"

namespace esep { namespace base {

ErrorManager::Error_ptr ErrorManager::create(communication::IRecipient *handler, communication::Packet_ptr packet)
{
	typedef communication::Message::Error Error;

	if(!packet->message().is<Error>())
	{
		MXT_THROW_EX(IncorrectMessageException);
	}

	switch(auto e = packet->message().as<Error>())
	{
		case Error::SERIAL:
		case Error::CONFIG:
			return Error_ptr(new error::Irrecoverable(handler, e));

		case Error::ESTOP:
			return Error_ptr(new error::Estop(handler));

		case Error::RAMP_FULL:
			return Error_ptr(new error::Ramp(handler));

		case Error::WARNING:
			return Error_ptr(new error::Warning(handler));

		default:
			MXT_LOG_ERROR("Received error", e, " that is not covered by ErrorManager!");
			MXT_THROW_EX(IncorrectMessageException);
			break;
	}
}

}}
