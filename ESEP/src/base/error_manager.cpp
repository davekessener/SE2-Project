
#include "base/error_manager.h"
#include "base/error/ramp_error.h"

namespace esep { namespace base {

std::unique_ptr<IManager> ErrorManager::create(communication::IRecipient *handler, communication::Packet::Message msg)
{
	typedef communication::Packet::Message Message;

	switch (msg)
	{
	case Message::RAMP_FULL:
		return std::unique_ptr<IManager>(new RampError(handler));
		break;
	default:
		MXT_THROW_EX(ErrorManager::IncorrectMessageException);
	}
}

}}
