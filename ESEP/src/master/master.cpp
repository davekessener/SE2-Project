#include "master/master.h"

#include "master/hsm/base.h"
#include "master/hsm/run.h"
#include "master/hsm/error.h"

#include "master/plugin/default.h"

#include "lib/logger.h"

namespace esep { namespace master {

Master::Master(IRecipient *com)
	: mCom(com)
	, mLogic(this, this)
{
	mPlugins.push_back(new plugin::Default);
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

	for(auto& p : mPlugins)
	{
		float t = p->match(data);

		if(!m || t > c)
		{
			m = p;
			c = t;
		}
	}

	if(item.plugin() && item.plugin() != m)
	{
		MXT_LOG_WARN("Conflicting types for item ", item.ID(), ": previously determined to be ", item.plugin()->type(), ", now it's ", m->type(), "!");
		item.action(Plugin::Action::TOSS);
	}

	item.plugin(m);
	item.data().push_back(data);
}

void Master::evaluate(Item& item, const history_t& history)
{
	item.action(item.plugin()->decide(history));
}

void Master::send(Location l, msg_t msg)
{
	mCom->accept(std::make_shared<Packet>(Location::MASTER, l, msg));
}

}}
