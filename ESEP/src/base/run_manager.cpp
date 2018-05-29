#include <memory>
#include "run_manager.h"
#include "lib/petri_net.h"
#include "lib/timer.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include "system.h"
#include "run/time_controller.h"
#include "run/types.h"
#include "data/location_data.h"
#include "data/data_point.h"

namespace esep { namespace base {

#define MXT_P_NR_STATES	14
#define MXT_CAST(t)		static_cast<uint8_t>(t)

RunManager::RunManager(communication::IRecipient* m, ConfigObject * c)
	:	mMaster(m)
	,	mConfigData(c)
	,	mTimeCtrl([this](TimerEvent e){ this->sendMessageWithData(Location::BASE, runMessage_t::TIMER, std::make_shared<data::RunManagerTimer>(e)); })
	,	mLogic(MXT_P_NR_STATES, Auto::FIRE)
{
	initLogic();
}

RunManager::~RunManager()
{
}

void RunManager::enter()
{
	mTimeCtrl.resumeAllTimer();
}

void RunManager::leave()
{
	mTimeCtrl.pauseAllTimer();
}

void RunManager::handle(Event e)
{
	run::HalEvent he;

	switch(e)
	{
		case(Event::LB_START):
			he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START) ? run::HalEvent::LB_START : run::HalEvent::I_LB_START;
			break;

		case(Event::LB_HEIGHTSENSOR):
			he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR) ? run::HalEvent::LB_HS : run::HalEvent::I_LB_HS;
			break;

		case(Event::LB_SWITCH):
			he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH) ? run::HalEvent::LB_SWITCH : run::HalEvent::I_LB_SWITCH;
			break;

		case(Event::LB_RAMP):
			he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP) ? run::HalEvent::LB_RAMP : run::HalEvent::I_LB_RAMP;
			break;

		case(Event::LB_END):
			he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END) ? run::HalEvent::LB_END : run::HalEvent::I_LB_END;
			break;

		default:
			return;
	}

	mLogic.process(he);
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
				HAL_MOTOR.start();
				break;

		case(MXT_CAST(runMessage_t::SUSPEND)):
				HAL_MOTOR.stop();
				break;

		case(MXT_CAST(runMessage_t::TIMER)):
				for (auto& d : *p)
				{
					if(d->type() == data::DataPoint::Type::RUN_MANAGER_TIMER)
					{
						//takes timer event
						mLogic.process((static_cast<data::RunManagerTimer&>(*d).event()));
					}
				}
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

void RunManager::sendMasterMessage(runMessage_t msg)
{
	mMaster->accept(std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, msg));
}

void RunManager::sendMessageWithData(Location target, runMessage_t msg, data::Data_ptr data)
{
	auto packet = std::make_shared<communication::Packet>(Location::BASE, target, msg);
	packet->addDataPoint(data);
	mMaster->accept(packet);
}

}
}
