#include "run_manager.h"
#include "lib/petri_net.h"
#include "lib/timer.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include "system.h"

namespace esep { namespace base {

#define MXT_P_NR_STATES	14
#define MXT_CAST(t)		static_cast<uint8_t>(t)

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
		//takes only messages of the type Run
		Message::Run runM = m.as<Message::Run>();
		switch(MXT_CAST(runM))
		{
		case(MXT_CAST(Message::Run::RESUME)):
				HAL_MOTOR.right();
				break;

		case(MXT_CAST(Message::Run::SUSPEND)):
				HAL_MOTOR.stop();
				break;

		//if its not resume or suspend, proceed this msg to logic
		default:
				mLogic.process(runM);
		}
	}
	else
	{
		MXT_LOG_WARN("Received unexpected packet { Source:", lib::hex<8>(p->source()), ", Target:", lib::hex<8>(p->target()), ", Msg: ", lib::hex<8>(m.as<Message::Run>()), "}");
	}
}

}
}
