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

void Packet::serialize(lib::ByteStream& bs)
{
	for(auto &datp : mDatapoints)
	{
		datp.get()->serialize(bs);
	}
}

}
}
