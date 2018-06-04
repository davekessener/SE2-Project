#include <memory>
#include <tuple>

#include "run_manager.h"

#include "lib/petri_net.h"
#include "lib/timer.h"
#include "lib/logger.h"
#include "lib/utils.h"

#include "run/time_controller.h"
#include "run/types.h"

#include "data/location_data.h"
#include "data/data_point.h"

#include "system.h"
#include "hal.h"

namespace esep { namespace base {

#define MXT_P_NR_STATES			14
#define MXT_CAST(t)				static_cast<uint8_t>(t)
#define MXT_ITEM_DUR			500
#define MXT_FINISHED			0
#define MXT_FIRSTTIME			1
#define MXT_HM					2

RunManager::RunManager(communication::IRecipient* m, ConfigObject * c)
	:	mMaster(m)
	,	mConfigData(c)
	,	mTimeCtrl([this](TimerEvent e){ this->sendMessageWithData(Location::BASE, Message::Run::TIMER, std::make_shared<data::RunManagerTimer>(e)); })
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

		case(Event::HEIGHT_SENSOR):
			takeMeasurement();
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

void RunManager::takeMeasurement()
{
	uint16_t hval = HAL_HEIGHT_SENSOR.measure();
	uint64_t hvalStamp = lib::Timer::instance().elapsed();


	if(mHeightMapBuffer.empty() || std::get<MXT_FINISHED>(mHeightMapBuffer.back()))
	{
		heightMap_ptr hm(new data::HeightMap);
		mHeightMapBuffer.emplace_back(std::make_tuple(false, hvalStamp, hm));
		std::get<MXT_HM>(mHeightMapBuffer.back())->addHeightValue(0, hval);
	}
	else
	{
		if(hval == 0)
		{
			std::get<MXT_FINISHED>(mHeightMapBuffer.back()) = true;
		}
		else
		{
			std::get<MXT_HM>(mHeightMapBuffer.back())->addHeightValue(timeDiff(std::get<MXT_FIRSTTIME>(mHeightMapBuffer.back()), hvalStamp), hval);
		}
	}
}

uint64_t RunManager::timeDiff(uint64_t oldstamp, uint64_t currstamp)
{
	return currstamp - oldstamp;
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
				HAL_MOTOR.start();
				mTimeCtrl.resumeAllTimer();
				break;

		case(MXT_CAST(Message::Run::SUSPEND)):
				HAL_MOTOR.stop();
				mTimeCtrl.pauseAllTimer();
				break;

		case(MXT_CAST(Message::Run::TIMER)):
				for (data::Data_ptr d : *p)
				{
					if(d->type() == data::DataPoint::Type::RUN_MANAGER_TIMER)
					{
						data::RunManagerTimer * data = dynamic_cast<data::RunManagerTimer *>(d.get());
						mLogic.process(data->event());
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
		MXT_LOG_WARN("Received unexpected packet { Source:", lib::hex<8>(p->source()), ", Target:", lib::hex<8>(p->target()), ", Msg: ", lib::hex<8>(m.as<Message::Run>()), "}");
	}
}

void RunManager::sendMasterMessage(Message::Run msg)
{
	mMaster->accept(std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, msg));
}

void RunManager::sendMessageWithData(Location target, Message::Run msg, data::Data_ptr data)
{
	auto packet = std::make_shared<communication::Packet>(Location::BASE, target, msg);
	packet->addDataPoint(data);
	mMaster->accept(packet);
}

void RunManager::sendItemInfo(data::Data_ptr hm, data::Data_ptr metal)
{
	auto packet = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::Run::ANALYSE);
	packet->addDataPoint(hm);
	packet->addDataPoint(metal);
	mMaster->accept(packet);
}

}
}
