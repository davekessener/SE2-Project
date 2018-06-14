#include "master/master.h"

#include "master/hsm/base.h"
#include "master/hsm/run.h"
#include "master/hsm/error.h"

#include "lib/logger.h"

#include "hal.h"

namespace esep { namespace master {

namespace
{
	std::string item_type_to_s(Plugin::Type t)
	{
		switch(t)
		{
		case Plugin::Type::CODED_000:
			return "CODED (000)";

		case Plugin::Type::CODED_001:
			return "CODED (001)";

		case Plugin::Type::CODED_010:
			return "CODED (010)";

		case Plugin::Type::CODED_011:
			return "CODED (011)";

		case Plugin::Type::CODED_100:
			return "CODED (100)";

		case Plugin::Type::CODED_101:
			return "CODED (101)";

		case Plugin::Type::CODED_110:
			return "CODED (110)";

		case Plugin::Type::CODED_111:
			return "CODED (111)";

		case Plugin::Type::FLAT:
			return "FLAT";

		case Plugin::Type::HOLLOW:
			return "HOLLOW (PLASTIC)";

		case Plugin::Type::HOLLOW_METAL:
			return "HOLLOW (METAL)";

		case Plugin::Type::UPSIDEDOWN:
			return "UPSIDE-DOWN";

		case Plugin::Type::UNKNOWN:
			return "UNKNOWN";

		case Plugin::Type::DEFAULT:
			return "DEFAULT";
		}

		return "ERROR";
	}
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

	MXT_LOG_INFO("Master received ", p);

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
			MXT_LOG_WARN("Conflicting types for item ", item.ID(), ": previously determined to be ", item.plugin()->type(), ", now it's ", m->type(), "!");

			item.action(Plugin::Action::TOSS);
		}

		item.plugin(m);

		HAL_CONSOLE.println("Identified Item #", item.ID(), ": ", item_type_to_s(m->type()), " on module ", item.location() == Packet::Location::BASE_S ? 2 : 1);
	}
	else
	{
		MXT_LOG_WARN("Could not identify item #", lib::hex<32>(item.ID()), "!");

		item.action(Plugin::Action::TOSS);
	}
}

void Master::evaluate(Item& item, const history_t& history)
{
	auto keep = [this, &item](void) {
		mCom->accept(std::make_shared<Packet>(Location::MASTER, item.location(), Message::Run::KEEP_NEXT));
	};

	auto toss = [this, &item, &keep](void) {
		if(mLogic.isRampFull(item.location()))
		{
			if(item.location() == Location::BASE_M)
			{
				item.action(Plugin::Action::TOSS_S);

				keep();
			}
			else
			{
				mCom->accept(std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Run::RAMP_FULL));
			}
		}
	};

	if(item.plugin())
	{
		item.action(item.plugin()->decide(history));

		switch(item.action())
		{
		case Plugin::Action::KEEP:
			keep();
			break;

		case Plugin::Action::TOSS:
			toss();
			break;

		case Plugin::Action::TOSS_M:
			if(item.location() == Location::BASE_M)
			{
				toss();
			}
			else
			{
				keep();
			}
			break;

		case Plugin::Action::TOSS_S:
			if(item.location() == Location::BASE_S)
			{
				toss();
			}
			else
			{
				keep();
			}
			break;

		default:
			MXT_LOG_WARN("Indecisive item #", (int)item.ID());
			break;
		}
	}
	else
	{
		toss();
	}
}

}}
