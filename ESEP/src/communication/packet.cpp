/*
 * message.cpp
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */




#include "communication/packet.h"
#include "data/data_manager.h"

namespace esep { namespace communication {

Packet::Packet(Location src, Location trg, Message msg)
{
	mSource = src;
	mTarget = trg;
	mMessage = msg;
}


void Packet::serialize(lib::ByteStream& bs)
{
	bs << mMessage << mSource << mTarget;


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
	 bs >> messag >> sourc >> targe;

	 std::shared_ptr<Packet> pakptr(new Packet(sourc, targe, messag));

	while(!bs.empty())
	{
		// wo ist hier der Fehler?
		pakptr->addDataPoint(data::DataManager::deserialize(bs));
	}
	return pakptr;

}

void Packet::addDataPoint(std::shared_ptr<data::DataPoint>)
{
	//addDatapoint(data::Datapoint* h)
	//std::shared_ptr<data::Datapoint> b(*h);
	//mDatapoints.pushback(b);
	throw Packet::NotImplException("TODO Implementation DataPoint");
}

}
}