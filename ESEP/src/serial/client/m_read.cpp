#include "serial/client/m_read.h"

namespace esep { namespace serial { namespace modules {

void Reader::process(packet::packet_ptr p)
{
	switch(p->getType())
	{
		case Type::SDP:
		case Type::MDP:
		case Type::LDP:
			processDataPacket(p);
			break;
		case Type::ACK_OK:
		case Type::ACK_ERR:
			mConnection.acknowledge(p->getID(), p->getType());
			break;
	}
}

void Reader::processDataPacket(packet::packet_ptr ptr)
{
	packet::Base_Data& p(static_cast<packet::Base_Data&>(*ptr));
	packet::packet_ptr a;

	if(!p.checkIntegrity())
	{
		a.reset(new packet::Answer<Type::ACK_ERR>(p.getID()));
	}
	else
	{
		a.reset(new packet::Answer<Type::ACK_OK>(p.getID()));

		if(recordPacket(ptr))
		{
			mChain << lib::byte_stream::insert_all(p.getData());

			if(!p.isChained())
			{
				mStorage.insert(mChain);

				mChain.clear();
			}
		}
	}

	mConnection.send(a);
}

bool Reader::recordPacket(packet::packet_ptr ptr)
{
	for(const auto& p : mHistory)
	{
		if(p->getID() == ptr->getID())
		{
			return false;
		}
	}

	mHistory->insert(ptr);

	return true;
}

}}}
