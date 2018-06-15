#include "base/run_manager.h"

#include "lib/logger.h"

#define MXT_P_NR_STATES			16
#define MXT_CAST(t)				static_cast<uint8_t>(t)
#define MXT_FINISHED			0
#define MXT_FIRSTTIME			1
#define MXT_HM					2

namespace esep { namespace base {

RunManager::RunManager(communication::IRecipient* m, ConfigObject * c)
	:	mMaster(m)
	,	mConfig(c)
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
	HAL_LIGHTS.turnOn(Light::GREEN);

	MXT_LOG_INFO("Entering RunManager.");
}

void RunManager::leave()
{
	suspend();

	HAL_LIGHTS.turnOff(Light::GREEN);
	HAL_SWITCH.close();

	MXT_LOG_INFO("Leaving RunManager.");
}

void RunManager::suspend(void)
{
	HAL_MOTOR.stop();
	mTimeCtrl.pauseAllTimer();
	mScanner.suspend();
}

void RunManager::resume(void)
{
	mScanner.resume();
	mTimeCtrl.resumeAllTimerDelayed(120);
	HAL_MOTOR.right();
	HAL_MOTOR.start();
}

void RunManager::handle(Event e)
{
	if(e != Event::HEIGHT_SENSOR)
	{
//		MXT_LOG_WARN("RUN: got hal event!: ", e);
	}
	run::HalEvent he;

	switch(e)
	{
		case(Event::BTN_STOP):
			if(HAL_BUTTONS.isPressed(Button::STOP))
				sendMasterMessage(Message::Run::REQUEST_STOP);
			return;

		case(Event::LB_START):
			he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START) ? run::HalEvent::LB_START : run::HalEvent::I_LB_START;
			break;

		case(Event::LB_HEIGHTSENSOR):
			he = HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR) ? run::HalEvent::LB_HS : run::HalEvent::I_LB_HS;
			break;

		case(Event::HEIGHT_SENSOR):
			takeMeasurement();
			return;

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
	mScanner.takeMeasurement(HAL_HEIGHT_SENSOR.measure());

//	uint16_t hval = HAL_HEIGHT_SENSOR.measure();
//	uint64_t hvalStamp = lib::Timer::instance().elapsed();
//
//	if(mHeightMapBuffer.empty() || std::get<MXT_FINISHED>(mHeightMapBuffer.back()))
//	{
//		if(hval)
//		{
//			heightMap_ptr hm(new data::HeightMap);
//			mHeightMapBuffer.emplace_back(std::make_tuple(false, hvalStamp, hm));
//			std::get<MXT_HM>(mHeightMapBuffer.back())->addHeightValue(0, hval);
//			MXT_LOG_INFO("Starting height map record.");
//		}
//	}
//	else
//	{
//		if(hval == 0)
//		{
//			std::get<MXT_FINISHED>(mHeightMapBuffer.back()) = true;
//
//			MXT_LOG_INFO("Finished height map.");
//		}
//		else
//		{
//			std::get<MXT_HM>(mHeightMapBuffer.back())->addHeightValue(timeDiff(std::get<MXT_FIRSTTIME>(mHeightMapBuffer.back()), hvalStamp), hval);
//		}
//	}
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

		switch(auto runM = m.as<Message::Run>())
		{
		case Message::Run::RESUME:
			resume();
			break;

		case Message::Run::SUSPEND:
			suspend();
			break;

		case Message::Run::TIMER:
			for (data::Data_ptr d : *p)
			{
				if(d->type() == data::DataPoint::Type::RUN_MANAGER_TIMER)
				{
					auto te = static_cast<data::RunManagerTimer&>(*d).event();
					mLogic.process(te);
				}
			}
			break;

		//if its not resume or suspend, proceed this msg to logic
		default:
			mLogic.process(runM);
			break;
		}
	}
	else
	{
		MXT_LOG_WARN("Received unexpected packet: ", p);
	}
}

void RunManager::sendMasterMessage(Message::Run msg)
{
	mMaster->accept(std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, msg));
}

void RunManager::sendMessageWithData(Location target, msg_t msg, data::Data_ptr data)
{
	auto packet = std::make_shared<communication::Packet>(Location::BASE, target, msg);
	packet->addDataPoint(data);
	mMaster->accept(packet);
}

}
}
