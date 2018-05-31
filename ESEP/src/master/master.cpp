#include "master/master.h"

#include "master/hsm/base.h"
#include "master/hsm/run.h"
#include "master/hsm/error.h"

#include "lib/logger.h"

namespace esep { namespace master {

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

	if(p->message().is<Message::Error>())
	{
		e = Event::ERROR;
	}
	else if(p->message().is<Message::Base>())
	{
		MXT_LOG_ERROR("Master received message intended for Base!: ", lib::hex<32>(e));
		return;
	}

	mLogic.process(e);
	mLogic.accept(p);
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
