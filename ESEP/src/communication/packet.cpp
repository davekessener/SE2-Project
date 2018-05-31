/*
 * message.cpp
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */




#include "communication/packet.h"
#include "data/data_manager.h"

namespace esep { namespace communication {

Packet::Packet(Location src, Location trg, msg_t msg)
	: mMessage(msg)
	, mTarget(trg)
	, mSource(src)
{
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
	Location s;
	Location t;
	msg_t m = Message::Master::FIXED;

	bs >> m >> s >> t;

	Packet_ptr p(new Packet(s, t, m));

	while(!bs.empty())
	{
		p->addDataPoint(data::DataManager::deserialize(bs));
	}

	return p;
}

void Packet::addDataPoint(std::shared_ptr<data::DataPoint> p)
{
	mDataPoints.push_back(p);
}

}}

std::ostream& operator<<(std::ostream& os, const esep::communication::Packet& p)
{
	return os << "{" << (int)p.source() << " -> " << (int)p.target() << ": " << esep::lib::hex<sizeof(p.message()) * 8>(p.message()) << "}";
}

