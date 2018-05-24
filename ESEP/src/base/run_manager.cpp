#include "run_manager.h"
#include "lib/petri_net.h"
#include "lib/timer.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include "system.h"
#include "run/time_controller.h"

namespace esep { namespace base {

#define MXT_P_NR_STATES	14
#define MXT_CAST(t)		static_cast<uint8_t>(t)

RunManager::RunManager(communication::IRecipient* m, ConfigObject * c)
	:	mMaster(m)
	,	mConfigData(c)
	,	mTimeCtrl([this](run::TimerEvent e){ acceptTimerEvent(e); })
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
	// TODO check if the LB is broken or not!
	mLogic.process(e);
}

void RunManager::accept(Packet_ptr p)
{
	msg_t m = p->message();

	if(m.is<Message>())
	{
		//takes only messages of the type Run
		Message runM = m.as<Message>();
		switch(MXT_CAST(runM))
		{
		case(MXT_CAST(Message::RESUME)):
				HAL_MOTOR.right();
				break;

		case(MXT_CAST(Message::SUSPEND)):
				HAL_MOTOR.stop();
				break;

		//if its not resume or suspend, proceed this msg to logic
		default:
				mLogic.process(runM);
		}
	}
	else
	{
		MXT_LOG_WARN("Received unexpected packet { Source:", lib::hex<8>(p->source()), ", Target:", lib::hex<8>(p->target()), ", Msg: ", lib::hex<8>(m.as<Message>()), "}");
	}
}

void RunManager::acceptTimerEvent(run::TimerEvent e)
{
	mLogic.process(e);
}

}
}
