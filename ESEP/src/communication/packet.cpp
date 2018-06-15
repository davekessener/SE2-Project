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
			case Message::Master::CONFIG:
				return "Master::CONFIG";
			case Message::Master::RUN:
				return "Master::RUN";
			case Message::Master::IDLE:
				return "Master::IDLE";
			case Message::Master::READY:
				return "Master::READY";
			case Message::Master::FIXED:
				return "Master::FIXED";
			case Message::Master::VALID:
				return "Master::VALID";
		}
		else if(m.is<Message::Base>())
			switch(m.as<Message::Base>())
		{
			case Message::Base::CONFIG:
				return "Base::CONFIG";
			case Message::Base::RUN:
				return "Base::RUN";
			case Message::Base::IDLE:
				return "Base::IDLE";
			case Message::Base::READY:
				return "Base::READY";
			case Message::Base::SHUTDOWN:
				return "Base::SHUTDOWN";
			case Message::Base::VALID:
				return "Base::VALID";
			case Message::Base::PRINT:
				return "Base::PRINT";
		}
		else if(m.is<Message::Config>())
			switch(m.as<Message::Config>())
		{
			case Message::Config::START:
				return "Config::START";
			case Message::Config::DONE:
				return "Config::DONE";
			case Message::Config::FAILED:
				return "Config::FAILED";
		}
		else if(m.is<Message::Run>())
			switch(m.as<Message::Run>())
		{
			case Message::Run::NEW_ITEM:
				return "Run::NEW_ITEM";
			case Message::Run::REACHED_END:
				return "Run::REACHED_END";
			case Message::Run::RAMP_FULL:
				return "Run::RAMP_FULL";
			case Message::Run::IN_HEIGHTSENSOR:
				return "Run::IN_HEIGHTSENSOR";
			case Message::Run::ANALYSE:
				return "Run::ANALYSE";
			case Message::Run::RESUME:
				return "Run::RESUME";
			case Message::Run::SUSPEND:
				return "Run::SUSPEND";
			case Message::Run::KEEP_NEXT:
				return "Run::KEEP_NEXT";
			case Message::Run::END_FREE:
				return "Run::END_FREE";
			case Message::Run::EXPECT_NEW:
				return "Run::EXPECT_NEW";
			case Message::Run::ITEM_APPEARED:
				return "Run::ITEM_APPEARED";
			case Message::Run::ITEM_DISAPPEARED:
				return "Run::ITEM_DISAPPEARED";
			case Message::Run::ITEM_STUCK:
				return "Run::ITEM_STUCK";
			case Message::Run::TIMER:
				return "Run::TIMER";
			case Message::Run::ITEM_REMOVED:
				return "Run::ITEM_REMOVED";
			case Message::Run::REQUEST_STOP:
				return "Run::REQUEST_STOP";
		}
		else if(m.is<Message::Error>())
			switch(m.as<Message::Error>())
		{
			case Message::Error::CONFIG:
				return "Error::CONFIG";
			case Message::Error::SERIAL:
				return "Error::SERIAL";
			case Message::Error::ITEM_APPEARED:
				return "Error::ITEM_APPEARED";
			case Message::Error::ITEM_DISAPPEARED:
				return "Error::ITEM_DISAPPEARED";
			case Message::Error::ITEM_STUCK:
				return "Error::ITEM_STUCK";
			case Message::Error::ESTOP:
				return "Error::ESTOP";
			case Message::Error::RAMP_FULL:
				return "Error::RAMP_FULL";
			case Message::Error::ANALYSE:
				return "Error::ANALYSE";
			case Message::Error::WARNING:
				return "Error::WARNING";
		}

		return "UNKNOWN";
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
	using namespace esep::communication;

	os << "{" << location_to_s(p.source()) << " [" << (int)p.source() << "] -> "
	   << location_to_s(p.target()) << " [" << (int)p.target() << "]: "
	   << msg_to_s(p.message()) << " [" << esep::lib::hex<sizeof(p.message()) * 8>(p.message()) << "]";

	bool f = true;

	for(const auto& d : p)
	{
		os << (f ? " {" : ", ") << d;
		f = false;
	}

	if(!f) os << "}";

	return os << "}";

//	return os << "{" << (int)p.source() << " -> " << (int)p.target() << ": " << esep::lib::hex<sizeof(p.message()) * 8>(p.message()) << "}";
}

