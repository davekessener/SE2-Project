#include "communication/master.h"

namespace esep { namespace communication {

Master::Master(Client_ptr p)
	: Base(std::move(p))
	, mMaster(nullptr)
{
}

Master::~Master(void)
{
	shutdown();
}

void Master::accept(Packet_ptr p)
{
	if(!mMaster)
	{
		MXT_THROW_EX(MissingBaseException);
	}

	switch(p->target())
	{
	case Packet::Location::MASTER:
		if(p->source() == Packet::Location::BASE)
		{
			p->source(Packet::Location::BASE_M);
		}
		Base::accept(p);
		break;

	case Packet::Location::BASE_M:
		p->target(Packet::Location::BASE);
		Base::accept(p);
		break;

	case Packet::Location::BASE_S:
		p->target(Packet::Location::BASE);
		send(p);
		break;

	case Packet::Location::BASE:
		Base::accept(p);
		send(p);
		break;
	}
}

void Master::process(Packet_ptr p)
{
	switch(p->target())
	{
	case Packet::Location::MASTER:
		mMaster->accept(p);
		break;

	case Packet::Location::BASE:
		base().accept(p);
		break;

	default:
		MXT_LOG("Processing invalid packet!");
		break;
	}
}

}}
