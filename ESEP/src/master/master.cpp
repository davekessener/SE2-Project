#include "master/master.h"

namespace esep { namespace master {

Master::Master(IRecipient *com)
	: mCom(com)
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
	}
}

}}
