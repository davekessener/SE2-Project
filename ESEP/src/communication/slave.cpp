#include "communication/slave.h"

#include "lib/logger.h"

namespace esep { namespace communication {

Slave::Slave(IRecipient *b, Client_ptr p)
	: Base(b, std::move(p))
{
}

Slave::~Slave(void)
{
	shutdown();
}

void Slave::accept(Packet_ptr p)
{
	switch(p->target())
	{
	case Packet::Location::MASTER:
		p->source(Packet::Location::BASE_S);
		send(p);
		break;

	case Packet::Location::BASE:
		Base::accept(p);
		break;

	default:
		MXT_LOG(lib::stringify("Encountered an invalid packet!"));
		break;
	}
}

void Slave::process(Packet_ptr p)
{
	if(p->target() == Packet::Location::BASE)
	{
		base().accept(p);
	}
	else
	{
		MXT_LOG("Processing invalid packet!");
	}
}

}}
