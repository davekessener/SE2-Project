
#include "base/error_manager.h"

#include "base/error/estop.h"
#include "base/error/irrecoverable.h"
#include "base/error/ramp.h"
#include "base/error/warning.h"
#include "base/error/item_appeared.h"
#include "base/error/item_disappeared.h"
#include "base/error/item_stuck.h"
#include "base/error/analyse.h"

#include "lib/logger.h"

namespace esep { namespace base {

typedef communication::Packet Packet;
typedef communication::Packet::Location Location;
typedef communication::Message Message;
typedef communication::Message::Error Error;
typedef data::Location::Type dloc_t;
typedef data::DataPoint::Type DataType;

ErrorManager::ErrorManager(communication::IRecipient * handler)
	: mHandler(handler)
	, mFixed(false)
{
}

void ErrorManager::enter()
{
	MXT_LOG_INFO("Entering ErrorManager.");
}

void ErrorManager::leave()
{
	if(static_cast<bool>(mCurrentError))
	{
		mCurrentError->leave();
		mCurrentError.reset();
	}

	mFixed = false;

	MXT_LOG_INFO("Leaving ErrorManager.");
}

void ErrorManager::accept(Packet_ptr packet)
{
	if (packet->target() == Location::MASTER)
	{
		if(packet->message() == Message::Master::FIXED)
		{
			mFixed = true;
		}

		mHandler->accept(packet);
	}
	else if(!packet->message().is<Error>())
	{
		MXT_LOG_WARN("Received erroneous message: ", packet);
	}
	else
	{
		Error_ptr m;

		switch (auto e = packet->message().as<Error>())
		{
			case Error::SERIAL:
			case Error::CONFIG:
				m.reset(new error::Irrecoverable(this, e));
				break;

			case Error::ESTOP:
				m.reset(new error::Estop(this));
				break;

			case Error::RAMP_FULL:
				m.reset(new error::Ramp(this));
				break;

			case Error::WARNING:
				m.reset(new error::Warning(this));
				break;

			case Error::ITEM_APPEARED:
				m.reset(new error::ItemAppeared(this, packet));
				break;

			case Error::ITEM_DISAPPEARED:
				m.reset(new error::ItemDisappeared(this));
				break;

			case Error::ITEM_STUCK:
				m.reset(new error::ItemStuck(this, packet));
				break;

			case Error::ANALYSE:
				m.reset(new error::Analyse(this, packet));
				break;

			default:
				MXT_LOG_ERROR("Received error", e, " that is not covered by ErrorManager!");
				MXT_THROW_EX(IncorrectMessageException);
				break;
		}

		if(!static_cast<bool>(mCurrentError))
		{
			mFixed = false;
			mCurrentError = std::move(m);
			mCurrentError->enter();
		}
		else if (m->priority() > mCurrentError->priority())
		{
			mFixed = false;
			mCurrentError->leave();
			mCurrentError = std::move(m);
			mCurrentError->enter();
		}
		else if(mFixed)
		{
			mHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::FIXED));
		}
	}
}

void ErrorManager::handle(Event event)
{
	mCurrentError->handle(event);
}

}}
