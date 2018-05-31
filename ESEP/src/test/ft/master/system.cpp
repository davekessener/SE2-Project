#include <map>
#include <iostream>

#include "test/ft/master/system.h"

#include "test/ft/master/forwarder.h"
#include "test/ft/master/consumer.h"

#include "data/data_point.h"
#include "data/metalsensor_data.h"
#include "data/location_data.h"

#include "lib/logger.h"

namespace esep { namespace test { namespace functional { namespace m {

typedef data::Data_ptr Data_ptr;

constexpr uint S_LENGTHS[] = {
	30,
	40,
	20,
	5
};

namespace
{
	std::vector<std::string> split(const std::string& line)
	{
		std::stringstream ss;
		std::vector<std::string> r;

		ss << line;

		while(true)
		{
			std::string s;

			ss >> s;

			if(s.empty()) break;

			r.push_back(s);
		}

		return r;
	}

	class SimplePlugin : public master::Plugin
	{
		typedef master::Plugin Plugin;
		typedef Plugin::Action Action;
		typedef Plugin::data_t data_t;
		typedef Plugin::history_t history_t;

		public:
			SimplePlugin(float f, Action a) : Plugin(Plugin::Type::DEFAULT), mMatch(f), mAction(a) { }

			float match(const data_t&) override { return mMatch; }
			Action decide(const history_t&) override { return mAction; }

		private:
			const float mMatch;
			const Action mAction;
	};

	typedef communication::Packet Packet;
	typedef communication::Packet_ptr Packet_ptr;
	typedef Packet::Location Location;

	std::string s_to_s(int s)
	{
		static const char * const STATES[] = { "START", "HS", "SWITCH", "END" };

		if(s >= 8) return "END";

		std::stringstream ss;

		ss << "M" << (s / 4) << "_" << STATES[s % 4];

		return ss.str();
	}

	std::string l_to_s(Location l)
	{
		switch(l)
		{
		case Location::MASTER:
			return "MASTER";
		case Location::BASE_M:
			return "BASE_M";
		case Location::BASE_S:
			return "BASE_S";
		case Location::BASE:
			return "BASE";
		}

		throw std::exception();
	}

	std::string m_to_s(communication::Packet::msg_t m)
	{
		static const char * const ENUMS[]    = { "Master", "Base", "Config", "Run", "Error" };
		static const char * const E_MASTER[] = { "CONFIG", "RUN", "IDLE", "READY", "FIXED" };
		static const char * const E_BASE[]   = { "CONFIG", "RUN", "IDLE", "READY", "SHUTDOWN"};
		static const char * const E_CONFIG[] = { "START", "DONE", "FAILED" };
		static const char * const E_ERROR[]    = { "CONFIG", "SERIAL", "ITEM_APPEARED", "ITEM_DISAPPEARED", "ESTOP", "RAMP_FULL", "WARNING" };
		static const char * const E_RUN[]  = { "NEW_ITEM", "REACHED_END", "RAMP_FULL", "IN_HEIGHTSENSOR", "ANALYSE", "RESUME", "SUSPEND", "KEEP_NEXT", "ITEM_REMOVED", "END_FREE", "EXPECT_NEW", "ITEM_APPEARED", "ITEM_DISAPPEARED" };
		static const char * const * const MESSAGES[] = { E_MASTER, E_BASE, E_CONFIG, E_RUN, E_ERROR };

		std::stringstream ss;
		uint v = static_cast<uint>(m);
		uint e = v >> 8, i = v & 0xff;

		ss << ENUMS[e] << "::" << MESSAGES[e][i];

		return ss.str();
	}

	std::string p_to_s(Packet_ptr p)
	{
		std::stringstream ss;

		ss << "{" << l_to_s(p->source()) << " -> " << l_to_s(p->target()) << ": " << m_to_s(p->message()) << " (" << lib::hex<16>(p->message()) << ")}";

		return ss.str();
	}
}

System::System(lib::Reader_ptr in, lib::Writer_ptr out)
	: mIn(std::move(in))
	, mOut(std::move(out))
	, mRunning(false)
	, mMaster(this, [this](const master::Item& item) {  }) // TODO
	, mNextItemID(0)
{
	mBeltActive[0] = mBeltActive[1] = false;

	auto on_insert = [this](Location l, Packet::msg_t msg) {
		return [this, l, msg](Item_ptr item) {
			mOutBuf.emplace_back(new Packet(l, Location::MASTER, msg));

			++mItems.at(item->ID());

			if(msg == Message::Run::ANALYSE)
			{
				for(auto& p : *item)
				{
					mOutBuf.back()->addDataPoint(p);
				}
			}
		};
	};

	auto on_remove = [this](Item_ptr item) {
		mOutBuf.emplace_back(new Packet(Location::BASE_S, Location::MASTER, Message::Run::END_FREE));

		mItems.erase(mItems.find(item->ID()));
		mOldItems.push_back(item);
	};

	mStations[S_M2_END]    = new Consumer( 1,    on_insert(Location::BASE_S, Message::Run::REACHED_END),     on_remove);
	mStations[S_M2_SWITCH] = new Forwarder(S_LENGTHS[S_M1_SWITCH], on_insert(Location::BASE_S, Message::Run::ANALYSE),         mStations[S_M2_END]);
	mStations[S_M2_HS]     = new Forwarder(S_LENGTHS[S_M1_HS],     on_insert(Location::BASE_S, Message::Run::IN_HEIGHTSENSOR), mStations[S_M2_SWITCH]);
	mStations[S_M2_START]  = new Forwarder(S_LENGTHS[S_M1_START],  on_insert(Location::BASE_S, Message::Run::NEW_ITEM),        mStations[S_M2_HS]);
	mStations[S_M1_END]    = new Forwarder(S_LENGTHS[S_M1_END],    on_insert(Location::BASE_M, Message::Run::END_FREE),        mStations[S_M2_START]);
	mStations[S_M1_SWITCH] = new Forwarder(S_LENGTHS[S_M1_SWITCH], on_insert(Location::BASE_M, Message::Run::ANALYSE),         mStations[S_M1_END]);
	mStations[S_M1_HS]     = new Forwarder(S_LENGTHS[S_M1_HS],     on_insert(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR), mStations[S_M1_SWITCH]);
	mStations[S_M1_START]  = new Forwarder(S_LENGTHS[S_M1_START],  on_insert(Location::BASE_M, Message::Run::NEW_ITEM),        mStations[S_M1_HS]);

	mMaster.add(master::Plugin_ptr(new SimplePlugin(0.0f, master::Plugin::Action::KEEP)));

	mOut->writeLine("=== START OF MASTER TEST ========================================================================");
}

System::~System(void)
{
	for(auto& p : mStations)
	{
		delete p;
	}

	mOut->writeLine("=================================================================================================");
}

void System::run(void)
{
	typedef void (System::*cb_fn)(const args_t&);

	std::map<std::string, cb_fn> commands;

	commands["h"] = &System::printHelp;
	commands["q"] = &System::quit;
	commands["n"] = &System::newItem;
	commands["s"] = &System::printStatus;
	commands["r"] = &System::switch2Run;
	commands["a"] = &System::advance;
	commands["l"] = &System::list;
	commands["d"] = &System::remove;

	mRunning = true;

	while(mRunning)
	{
		mOut->writeLine("?");

		args_t line = split(mIn->readLine());

		if(line.empty()) continue;

		auto i = commands.find(line.front());

		if(i == commands.end())
		{
			mOut->writeLine("Unknown command!");

			continue;
		}

		(this->*(i->second))(line);

		flush();
	}
}

void System::flush(void)
{
	decltype(mOutBuf) ob;
	decltype(mInBuf) ib;

	std::swap(ob, mOutBuf);
	std::swap(ib, mInBuf);

	for(auto& p : ob)
	{
		mOut->writeLine(lib::stringify("Sending ", p_to_s(p)));

		mMaster.accept(p);
	}

	for(auto& p : ib)
	{
		mOut->writeLine(lib::stringify("Received ", p_to_s(p)));

		if(p->message().is<Message::Run>())
			switch(p->message().as<Message::Run>())
		{
			case Message::Run::RESUME:
			case Message::Run::SUSPEND:
				mBeltActive[p->target() == Location::BASE_M ? 0 : 1] = (p->message() == Message::Run::RESUME);
				break;

			case Message::Run::KEEP_NEXT:
				mOutBuf.emplace_back(new Packet(p->target(), Location::MASTER, p->message()));
				break;

			default:
				MXT_LOG_WARN("Ignoring message!");
				break;
		}
	}

	if(!mOutBuf.empty() || !mInBuf.empty()) flush();
}

void System::accept(Packet_ptr p)
{
	switch(p->target())
	{
	case Location::BASE:
		mInBuf.emplace_back(new Packet(*p));
		mInBuf.back()->target(Location::BASE_M);
		mInBuf.emplace_back(new Packet(*p));
		mInBuf.back()->target(Location::BASE_S);
		break;

	case Location::BASE_M:
	case Location::BASE_S:
		mInBuf.push_back(p);
		break;

	case Location::MASTER:
		mOutBuf.push_back(p);
		break;
	}
}

void System::printHelp(const args_t& args)
{
	if(args.size() > 1)
	{
	}
	else
	{
		mOut->writeLine("Master Logic test system commands:");

		mOut->writeLine("h [command]  Prints help list.");
		mOut->writeLine("q            Terminates system.");
		mOut->writeLine("n [metal]    Creates a new item at the beginning of M1.");
		mOut->writeLine("s            Prints current system status.");
		mOut->writeLine("r            Switches to run manager.");
		mOut->writeLine("a            Advance items.");
		mOut->writeLine("l            List all items currently in the system.");
		mOut->writeLine("d id         Deletes item.");
	}
}

void System::quit(const args_t& args)
{
	mOut->writeLine("Shutting down ...");

	mRunning = false;
}

void System::newItem(const args_t& args)
{
	Item_ptr item(new Item(mNextItemID++));

	mItems[item->ID()] = 0;

	item->addData(Data_ptr(new data::MetalSensor(args.size() > 1 && args[1] == "metal")));

	mStations[S_M1_START]->insert(item);
}

void System::printStatus(const args_t& args)
{
	mOut->writeLine(lib::stringify("Items: ", mItems.size()));
	mOut->writeLine(lib::stringify("M1: ", mBeltActive[0] ? "ON" : "OFF"));
	mOut->writeLine(lib::stringify("M2: ", mBeltActive[1] ? "ON" : "OFF"));
}

void System::switch2Run(const args_t& args)
{
	mOutBuf.emplace_back(new Packet(Location::BASE_M, Location::MASTER, Message::Master::RUN));
}

void System::advance(const args_t& args)
#define MXT_MIN(a,b) (((a)<(b))?(a):(b))
{
	if(!mItems.empty())
	{
		uint t = -1;

		if(mBeltActive[0])
		{
			for(uint i = S_M1_START ; i <= S_M1_END ; ++i)
			{
				t = MXT_MIN(t, mStations[i]->remaining());
			}
		}

		if(mBeltActive[1])
		{
			for(uint i = S_M2_START ; i <= S_M2_END ; ++i)
			{
				t = MXT_MIN(t, mStations[i]->remaining());
			}
		}

		if(t < (uint)-1)
		{
			while(t--)
			{
				for(uint i1 = (mBeltActive[0] ? S_M1_START : S_M2_START), i2 = (mBeltActive[1] ? S_M2_END : S_M1_END) ; i1 != i2 ; ++i1)
				{
					mStations[i1]->tick();
				}
			}
		}
	}
}
#undef MXT_MIN

void System::list(const args_t&)
{
	mOut->writeLine("ID Position");

	for(const auto& p : mItems)
	{
		mOut->writeLine(lib::stringify(lib::hex<8>(p.first), ": ", s_to_s(p.second)));
	}
}

void System::remove(const args_t& args)
{
	if(args.size() < 2)
	{
		mOut->writeLine("Delete requires an argument!");

		return;
	}

	try
	{
		Item::id_t id = lib::lexical_cast<Item::id_t>(args.at(1));

		auto i = mItems.find(id);

		if(i == mItems.end())
		{
			mOut->writeLine("No such item!");

			return;
		}

		auto pos = i->second;

		if(pos >= 8)
		{
			mStations[S_M2_END]->tick();
		}
		else
		{
			mStations[pos]->remove(id);
			mItems.erase(i);

			Location src = pos / 4 ? Location::BASE_S : Location::BASE_M;
			auto p = new Packet(src, Location::MASTER, Message::Run::ITEM_REMOVED);

			if((pos % 4) != 2)
			{
				p->message(Message::Run::ITEM_DISAPPEARED);
				data::Location::Type l;

				switch(pos % 4)
				{
				case 0:
					l = data::Location::Type::LB_HEIGHTSENSOR;
					break;
				case 1:
					l = data::Location::Type::LB_SWITCH;
					break;
				case 3:
					p->source(Location::BASE_S);
					l = data::Location::Type::LB_START;
					break;
				default:
					MXT_LOG_WARN("Should not happen! ", pos);
					return;
				}

				p->addDataPoint(Data_ptr(new data::Location(l)));
			}

			mOutBuf.emplace_back(p);
		}
	}
	catch(const tml::string::BadCastException& e)
	{
		mOut->writeLine("Delete requires a valid id!");
	}
}

}}}}
