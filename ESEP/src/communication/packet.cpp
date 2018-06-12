#include "communication/packet.h"
#include "data/data_manager.h"

namespace esep { namespace communication {

namespace
{
	typedef communication::Packet Packet;
	typedef communication::Message Message;
	typedef Packet::Location Location;

	std::string location_to_s(Location l)
	{
		switch(l)
		{
		case Location::MASTER:
			return "MASTER";

		case Location::BASE:
			return "BASE";

		case Location::BASE_M:
			return "B(M)";

		case Location::BASE_S:
			return "B(S)";
		}

		return "ERR";
	}

	std::string msg_to_s(Packet::msg_t m)
	{
		if(m.is<Message::Master>())
			switch(m.as<Message::Master>())
		{
		}
		else if(m.is<Message::Base>())
			switch(m.as<Message::Base>())
		{
		}
		else if(m.is<Message::Config>())
			switch(m.as<Message::Config>())
		{
		}
		else if(m.is<Message::Error>())
			switch(m.as<Message::Error>())
		{
		}

		return "ERR";
	}
}

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

