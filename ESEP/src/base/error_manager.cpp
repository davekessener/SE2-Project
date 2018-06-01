
#include "base/error_manager.h"

#include "base/error/estop.h"
#include "base/error/irrecoverable.h"
#include "base/error/ramp.h"
#include "base/error/warning.h"
#include "base/error/item_appeared.h"

#include "lib/logger.h"

namespace esep { namespace base {

typedef communication::Packet::Location Location;
typedef communication::Message::Error Error;
typedef data::Location::Type dloc_t;
typedef data::DataPoint::Type DataType;

ErrorManager::ErrorManager(communication::IRecipient * handler)
	: mHandler(handler)
{

}

//ErrorManager::Error_ptr ErrorManager::create(communication::IRecipient *handler, communication::Packet_ptr packet)
//{
//	typedef communication::Message::Error Error;
//
//	if(!packet->message().is<Error>())
//	{
//		MXT_THROW_EX(IncorrectMessageException);
//	}
//
//	switch(auto e = packet->message().as<Error>())
//	{
//		case Error::SERIAL:
//		case Error::CONFIG:
//			return Error_ptr(new error::Irrecoverable(handler, e));
//
//		case Error::ESTOP:
//			return Error_ptr(new error::Estop(handler));
//
//		case Error::RAMP_FULL:
//			return Error_ptr(new error::Ramp(handler));
//
//		case Error::WARNING:
//			return Error_ptr(new error::Warning(handler));
//
//		default:
//			MXT_LOG_ERROR("Received error", e, " that is not covered by ErrorManager!");
//			MXT_THROW_EX(IncorrectMessageException);
//			break;
//	}
//}

void ErrorManager::enter()
{

}

void ErrorManager::leave()
{
	mCurrentError->leave();
	mCurrentError.reset();
}

void ErrorManager::accept(Packet_ptr packet)
{

	if (packet->target() == Location::MASTER)
	{
		mHandler->accept(packet);
	}
	else
	{
		Error_ptr m;
		switch (auto e = packet->message().as<Error>())
		{
			case Error::SERIAL:
			case Error::CONFIG:
				m = Error_ptr(new error::Irrecoverable(this, e));
				break;

			case Error::ESTOP:
				m = Error_ptr(new error::Estop(this));
				break;

			case Error::RAMP_FULL:
				m = Error_ptr(new error::Ramp(this));
				break;

			case Error::WARNING:
				m = Error_ptr(new error::Warning(this));
				break;

			case Error::ITEM_APPEARED:
				//TODO How to get only one DataPoint?
				for (auto& p : *packet)
				{
					if(p->type() == DataType::LOCATION)
					{
						m = Error_ptr(new error::ItemAppeared(this, dynamic_cast<data::Location&>(*p).location()));
						break;
					}
				}
				break;

			default:
				MXT_LOG_ERROR("Received error", e, " that is not covered by ErrorManager!");
				MXT_THROW_EX(IncorrectMessageException);
				break;
		}

		if(!static_cast<bool>(mCurrentError))
		{
			mCurrentError = std::move(m);
			mCurrentError->enter();
		}
		else if (m->priority() >= mCurrentError->priority())
		{
			mCurrentError->leave();
			mCurrentError = std::move(m);
			mCurrentError->enter();
		}
	}
}

void ErrorManager::handle(Event event)
{
	mCurrentError->handle(event);
}

}}
