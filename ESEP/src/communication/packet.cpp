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
	bs << mMessage << mSource << mTarget;

	for(auto &datp : mDataPoints)
	{
		datp.get()->serialize(bs);
	}
}

 std::shared_ptr<Packet> Packet::deserialize(lib::ByteStream& bs)
{

//	bs >> sourc >>  >> targe >> messag;
//	Packet pak = new Packet();
//
//	for(auto &datp : bs)
//	{
//
//	}

	 return nullptr;

}

}
}
