#include "master/hsm/error.h"

#include "lib/logger.h"

namespace esep { namespace master { namespace hsm {

void Error::handle(Packet_ptr p)
{
	if(p->message() == Message::Master::FIXED)
	{
	}
	else if(!p->message().is<Message::Error>())
	{
		MXT_LOG_WARN("Received an unintelligible message during error! ", p);
	}
	else switch(p->message().as<Message::Error>())
	{
	case Message::Error::CONFIG:
	case Message::Error::SERIAL:
	case Message::Error::ESTOP:
	case Message::Error::RAMP_FULL:
		mCom->accept(Packet_ptr(new Packet(Location::MASTER, Location::BASE, p->message())));
		break;

	case Message::Error::ITEM_APPEARED:
	case Message::Error::ITEM_DISAPPEARED:
	case Message::Error::ITEM_STUCK:
		if(p->source() != Location::BASE_M && p->source() != Location::BASE_S)
		{
			MXT_LOG_ERROR("Received a mangled packet! ", p);
		}
		else
		{
			mCom->accept(Packet_ptr(new Packet(Location::MASTER, p->source() == Location::BASE_M ? Location::BASE_S : Location::BASE_M, Message::Error::WARNING)));
			p.reset(new Packet(*p));
			p->target(p->source());
			p->source(Location::MASTER);
			mCom->accept(p);
		}
		break;

	case Message::Error::WARNING:
		MXT_LOG_WARN("Received WARNING from ", (int)p->source(), " for some reason!");
		break;
	}
}

}}}
