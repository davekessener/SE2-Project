/*
 * message.cpp
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */


//addDatapoint(data::Datapoint* h)
//std::shared_ptr<data::Datapoint> b(*h);
//mDatapoints.pushback(b);

#include "communication/packet.h"
#include "data/data_manager.h"

namespace esep { namespace communication {

Packet::Packet(Location src, Location trg, Message msg)
{
	mSource = src;
	mTarget = trg;
	mMessage = msg;
}

Packet::~Packet()
{
	delete this;
}


void Packet::serialize(lib::ByteStream& bs)
{
	bs << mMessage;
	bs << mSource;
	bs << mTarget;

	for(auto &datp : mDataPoints)
	{
		datp->serialize(bs);
	}
}

 std::shared_ptr<Packet> Packet::deserialize(lib::ByteStream& bs)
{
	 Location sourc;
	 Location targe;
	 Message messag;

	 // cast?
	 bs >> messag;
	 bs >> sourc;
	 bs >> targe;

	 std::shared_ptr<Packet> pakptr(new Packet(sourc, targe, messag));

	while(!bs.empty())
	{
		// wo ist hier der Fehler?
		//pakptr->addDataPoint(data::DataManager::deserialize(bs));
	}
	return pakptr;

}

}
}
