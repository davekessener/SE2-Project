
#include "base/error_manager.h"
#include "base/error/ramp_error.h"
#include "base/error/irrecoverable_error.h"
#include "base/error/estop_error.h"
#include "base/error/warning_error.h"

namespace esep { namespace base {

std::unique_ptr<IManager> ErrorManager::create(communication::IRecipient *handler, communication::Packet_ptr packet)
{
	typedef communication::Packet::Message Message;

	switch (packet->message())
	{
		case Message::ERROR_SERIAL:
		case Message::CONFIG_FAILED:
			return std::unique_ptr<IManager>(new IrrecoverableError(handler, packet->message()));
			break;
		case Message::ESTOP:
			return std::unique_ptr<IManager>(new EstopError(handler));
			break;
		case Message::RAMP_FULL:
			return std::unique_ptr<IManager>(new RampError(handler));
			break;
		case Message::WARNING:
			return std::unique_ptr<IManager>(new WarningError(handler));
			break;
		// TODO here: ItemError
		default:
			MXT_THROW_EX(ErrorManager::IncorrectMessageException);
	}
}

}}
