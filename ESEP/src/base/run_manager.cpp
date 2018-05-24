#include "run_manager.h"
#include "lib/petri_net.h"
#include "lib/timer.h"
#include "lib/logger.h"
#include "lib/utils.h"

namespace esep { namespace base {

#define MXT_P_NR_STATES	13

RunManager::RunManager(communication::IRecipient* m)
	:	mMaster(m)
	,	mLogic(MXT_P_NR_STATES, Auto::FIRE)
{
	initLogic();
}

RunManager::~RunManager()
{

}

void RunManager::enter()
{

}

void RunManager::leave()
{

}

void RunManager::handle(Event e)
{
	mLogic.process(e);
}

void RunManager::accept(Packet_ptr p)
{
	msg_t m = p->message();
	if(m.is<Message::Run>())
	{
		mLogic(m.as<Message::Run>());
	}
	else
	{
		MXT_LOG_WARN("Received unexpected pulse { Source:", lib::hex<8>(static_cast<uint8_t>(p->source)), ", Target:", lib::hex<8>(static_cast<uint8_t>(p->target)), ", Msg: ", lib::hex<8>(static_cast<uint8_t>(m.as<Message::Run>())), "}");
	}
}

}
}
