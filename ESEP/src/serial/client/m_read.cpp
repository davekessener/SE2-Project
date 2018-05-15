#include "serial/client/m_read.h"

namespace esep { namespace serial { namespace modules {

void Reader::process(packet::packet_ptr p)
{
	switch(p->getType())
	{
		case packet::Type::SDP:
		case packet::Type::MDP:
		case packet::Type::LDP:
			processDataPacket(p);
			break;
		case packet::Type::AP_OK:
		case packet::Type::AP_ERR:
			mConnection.acknowledge(p->getID(), p->getType());
			break;
		default:
			MXT_THROW("Invalid packet type processed (", static_cast<uint16_t>(p->getType()), "!");
	}
}

void Reader::processDataPacket(packet::packet_ptr ptr)
{
	packet::Base_Data& p(static_cast<packet::Base_Data&>(*ptr));

	if(!p.checkIntegrity())
	{
		mLastAck.reset(new packet::Answer<packet::Type::AP_ERR>(p.getID()));
	}
	else
	{
		mLastAck.reset(new packet::Answer<packet::Type::AP_OK>(p.getID()));

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

	mConnection.send(mLastAck);
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

	mHistory.insert(ptr);

	return true;
}

types::buffer_t Reader::get(void)
{
	return mStorage.remove();
}

void Reader::reset(void)
{
	if(static_cast<bool>(mLastAck))
	{
		mConnection.send(mLastAck);
	}
}

}}}
