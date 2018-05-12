
#include "base/error_manager.h"
#include "base/error/ramp_error.h"
#include "base/error/irrecoverable_error.h"

namespace esep { namespace base {

std::unique_ptr<IManager> ErrorManager::create(communication::IRecipient *handler, communication::Packet_ptr packet)
{
	typedef communication::Packet::Message Message;
	std::unique_ptr<IManager> error;

	switch (packet->message())
	{
		case Message::ERROR_SERIAL:
			error =  std::unique_ptr<IManager>(new IrrecoverableError(handler));
			error->accept(packet);
			break;
		case Message::CONFIG_FAILED:
			error = std::unique_ptr<IManager>(new IrrecoverableError(handler));
			error->accept(packet);
			break;
		case Message::RAMP_FULL:
			error = std::unique_ptr<IManager>(new RampError(handler));
			break;
		default:
			MXT_THROW_EX(ErrorManager::IncorrectMessageException);
	}

	return error;
}

}}
