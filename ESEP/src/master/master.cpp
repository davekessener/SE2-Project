#include <fstream>

#include "master/master.h"

#include "master/hsm/base.h"
#include "master/hsm/run.h"
#include "master/hsm/error.h"

#include "data/metalsensor_data.h"
#include "data/heightmap_data.h"
#include "data/message_data.h"

#include "master/plugin/hausdorff.h"

#include "lib/logger.h"

#include "hal.h"

namespace esep { namespace master {

namespace
{
//	analyse::Profiles::Item type_to_item(Plugin::Type t)
//	{
//		switch(t)
//		{
//		case Plugin::Type::CODED_001:
//			return analyse::Profiles::Item::CODED_001;
//
//		case Plugin::Type::CODED_010:
//			return analyse::Profiles::Item::CODED_010;
//
//		case Plugin::Type::CODED_011:
//			return analyse::Profiles::Item::CODED_011;
//
//		case Plugin::Type::CODED_100:
//			return analyse::Profiles::Item::CODED_100;
//
//		case Plugin::Type::CODED_101:
//			return analyse::Profiles::Item::CODED_101;
//
//		case Plugin::Type::CODED_110:
//			return analyse::Profiles::Item::CODED_110;
//
//		case Plugin::Type::CODED_111:
//			return analyse::Profiles::Item::CODED_111;
//
//		case Plugin::Type::CODED_000:
//			return analyse::Profiles::Item::CODED_000;
//
//		case Plugin::Type::HOLLOW:
//		case Plugin::Type::HOLLOW_METAL:
//			return analyse::Profiles::Item::HOLLOW;
//
//		case Plugin::Type::FLAT:
//			return analyse::Profiles::Item::FLAT;
//
//		case Plugin::Type::UPSIDEDOWN:
//			return analyse::Profiles::Item::UPSIDEDOWN;
//
//		default:
//			throw 0;
//		}
//	}
//
//	template<typename C>
//	void save_data(uint id, Plugin::Type t, const C& c)
//	{
//		for(const auto& d : c)
//		{
//			if(d->type() == data::DataPoint::Type::HEIGHT_MAP)
//			{
//				std::ofstream out(lib::stringify("item_data_", lib::hex<16>(id), ".txt"));
//				plugin::Hausdorff::processor_t p;
//				auto r = p.use(dynamic_cast<data::HeightMap *>(&*d)).get<plugin::Hausdorff::Station::NORMALIZED>();
//
//				out << "[";
//				for(const auto& e : r)
//				{
//					out << e.x() << " " << e.y() << "; ";
//				}
//				out << "];" << std::endl;
//
//				out << "[";
//				for(const auto& e : analyse::Profiles::get(type_to_item(t)))
//				{
//					out << e.x() << " " << e.y() << "; ";
//				}
//				out << "];" << std::endl;
//
//				out.flush();
//				out.close();
//
//				return;
//			}
//		}
//	}
}

Master::Master(IRecipient *com, item_handler_fn f)
	: mCom(com)
	, mCallback(f)
	, mLogic(com, this)
{
}

Master::~Master(void)
{
	for(auto& p : mPlugins)
	{
		delete p;
	}
}

void Master::accept(Packet_ptr p)
{
	event_t e = Event::fromPacket(p);

//	MXT_LOG_INFO("Master received ", p);

	if(p->message().is<Message::Error>())
	{
		e = Event::ERROR;
	}
	else if(p->message().is<Message::Base>())
	{
		MXT_LOG_ERROR("Master received message intended for Base!: ", lib::hex<32>(e));
		return;
	}

	try
	{
		mLogic.process(e);
		mLogic.accept(p);
	}
	MXT_CATCH_ALL_STRAY
}

void Master::analyse(Item& item, const data_t& data)
{
	Plugin *m = nullptr;
	float c = 0.0f;

	item.data().push_back(data);

	for(auto& p : mPlugins)
	{
		float t = p->match(data);

		if(!m || t > c)
		{
			m = p;
			c = t;
		}
	}

	if(m)
	{
		if(item.plugin() && item.plugin() != m)
		{
			MXT_LOG_WARN("Conflicting types for item ", item.ID(), ": previously determined to be ", Plugin::type_to_s(item.plugin()->type()), ", now it's ", Plugin::type_to_s(m->type()), "!");

			item.action(Action::TOSS);
		}

		item.plugin(m);

//		save_data(item.ID(), m->type(), data);

		MXT_LOG_INFO("Identified Item #", item.ID(), ": ", Plugin::type_to_s(m->type()), " on module ", item.location() == Packet::Location::BASE_S ? 2 : 1);

		message(item.location(), lib::stringify("ITEM #", item.ID(), ": ", Plugin::type_to_s(m->type())));
	}
	else
	{
		MXT_LOG_WARN("Could not identify item #", lib::hex<32>(item.ID()), "!");

		item.action(Action::TOSS);
	}
}

void Master::message(Location t, const std::string& msg)
{
	auto p = std::make_shared<Packet>(Location::MASTER, t, Message::Base::PRINT);

	p->addDataPoint(Data_ptr(new data::Message(msg)));

	mCom->accept(p);
}

void Master::evaluate(Item& item, const history_t& history)
{
	auto keep = [this, &item](void) {
		mCom->accept(std::make_shared<Packet>(Location::MASTER, item.location(), Message::Run::KEEP_NEXT));
	};

	auto toss = [this, &item, &keep](void) {
		if(item.action() == Action::TOSS)
		{
			item.action(item.location() == Location::BASE_M ? Action::TOSS_M : Action::TOSS_S);
		}

		if(item.action() == Action::TOSS_M && mLogic.isRampFull(Location::BASE_M))
		{
			item.action(Action::TOSS_S);

			keep();
		}
		else if(item.action() == Action::TOSS_S && mLogic.isRampFull(Location::BASE_S))
		{
			if(item.location() == Location::BASE_S)
			{
				mCom->accept(std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Run::RAMP_FULL));
			}
		}
		else if(item.action() == Action::TOSS_M && item.location() == Location::BASE_S)
		{
			MXT_LOG_FATAL("Trying to remove an item on M1 from M2!");
		}
		else if(item.action() == Action::TOSS_S && item.location() == Location::BASE_M)
		{
			keep();
		}
	};

	if(item.plugin())
	{
		item.action(item.plugin()->decide(history));

		switch(item.action())
		{
		case Action::KEEP:
			keep();
			break;

		case Action::TOSS:
		case Action::TOSS_M:
		case Action::TOSS_S:
			toss();
			break;

		default:
			MXT_LOG_WARN("Indecisive item #", (int)item.ID(), " wanting ", Plugin::action_to_s(item.action()));
			break;
		}
	}
	else
	{
		toss();
	}
}

}}
