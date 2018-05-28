#include "run_manager.h"
#include "lib/petri_net.h"
#include "lib/timer.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include "system.h"
#include "run/time_controller.h"
#include "run/types.h"

namespace esep { namespace base {

#define MXT_P_NR_STATES	14
#define MXT_CAST(t)		static_cast<uint8_t>(t)

RunManager::RunManager(communication::IRecipient* m, ConfigObject * c)
	:	mMaster(m)
	,	mConfigData(c)
	,	mTimeCtrl([this](run::TimerEvent e, run::State s){ acceptTimerEvent(e); this->mTimeCtrl.deleteTimer(s); })
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
	switch(MXT_CAST(e))
	{
		case(MXT_CAST(halEvent_t::LB_START)):
			{
				run::HalEvent he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START) ? run::HalEvent::LB_START : run::HalEvent::I_LB_START;
				mLogic.process(he);
				break;
			}

		case(MXT_CAST(halEvent_t::LB_HEIGHTSENSOR)):
			{
				run::HalEvent he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR) ? run::HalEvent::LB_HS : run::HalEvent::I_LB_HS;
				mLogic.process(he);
				break;
			}

		case(MXT_CAST(halEvent_t::LB_SWITCH)):
			{
				run::HalEvent he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH) ? run::HalEvent::LB_SWITCH : run::HalEvent::I_LB_SWITCH;
				mLogic.process(he);
				break;
			}

		case(MXT_CAST(halEvent_t::LB_RAMP)):
			{
				run::HalEvent he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP) ? run::HalEvent::LB_RAMP : run::HalEvent::I_LB_RAMP;
				mLogic.process(he);
				break;
			}

		case(MXT_CAST(halEvent_t::LB_END)):
			{
				run::HalEvent he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END) ? run::HalEvent::LB_END : run::HalEvent::I_LB_END;
				mLogic.process(he);
				break;
			}

		default:
			{
				MXT_THROW_E(UndefinedEventException, lib::stringify("There is no definition for the hal::HAL::Event ", MXT_CAST(e)));
			}
	}
}

void RunManager::accept(Packet_ptr p)
{
	msg_t m = p->message();

	if(m.is<runMessage_t>())
	{
		//takes only messages of the type Run
		runMessage_t runM = m.as<runMessage_t>();
		switch(MXT_CAST(runM))
		{
		case(MXT_CAST(runMessage_t::RESUME)):
				HAL_MOTOR.right();
				break;

		case(MXT_CAST(runMessage_t::SUSPEND)):
				HAL_MOTOR.stop();
				break;

		//if its not resume or suspend, proceed this msg to logic
		default:
				mLogic.process(runM);
		}
	}
	else
	{
		MXT_LOG_WARN("Received unexpected packet { Source:", lib::hex<8>(p->source()), ", Target:", lib::hex<8>(p->target()), ", Msg: ", lib::hex<8>(m.as<runMessage_t>()), "}");
	}
}

void RunManager::acceptTimerEvent(run::TimerEvent e)
{
	mLogic.process(e);
}

}
}
