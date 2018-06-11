#include <tuple>

#include "base/run_manager.h"

#include "lib/timer.h"
#include "lib/timer/manager.h"

#include "communication/packet.h"
#include "communication/message.h"

#include "base/run/types.h"
#include "lib/logger.h"
#include "data/data_point.h"
#include "data/location_data.h"
#include "data/heightmap_data.h"
#include "data/metalsensor_data.h"
#include "data/run_manager_timer_data.h"

#include "system.h"
#include "hal.h"


namespace esep { namespace base {

#define MXT_CAST(t)				static_cast<uint8_t>(t)
#define MXT_SHARE(T, V)			data::Data_ptr(new T(V))
#define MXT_TOLERANCE_MULT		4
#define MXT_HM					2
#define MXT_FINISHED			0

void RunManager::initLogic()
{
	//--------- Eingabe bis zur Hoehenmessung
	//EXPECT_NEW
	mLogic.transition(Message::Run::EXPECT_NEW,
			{},
			{{MXT_CAST(State::BF_LB_START), 1}},
			[this](void)
			{
				mTimeCtrl.setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, mConfig->maxHandOverTime());
			});
	//TIMER_EXPECT_NEW
	mLogic.transition(TimerEvent::EXPECT_NEW,
			{{MXT_CAST(State::BF_LB_START), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_LB_START);
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_START));
			});
	//LB_START_1
	mLogic.transition(run::HalEvent::LB_START,
			{},
			{{MXT_CAST(State::IN_LB_START), 1}},
			[this](void)
			{
				sendMasterMessage(Message::Run::NEW_ITEM);
				mTimeCtrl.setTimer(State::IN_LB_START, TimerEvent::START_1, computeMaxTime(mConfig->itemInLB()));
			});
	//LB_START_2
	mLogic.transition(run::HalEvent::LB_START,
			{{MXT_CAST(State::BF_LB_START), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_LB_START);
			});
	//TIMER_START_1
	mLogic.transition(TimerEvent::START_1,
			{{MXT_CAST(State::IN_LB_START), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_START);
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_STUCK, MXT_SHARE(data::Location, data::Location::Type::LB_START));
			});
	//!LB_START
	mLogic.transition(run::HalEvent::I_LB_START,
			{{MXT_CAST(State::IN_LB_START), 1}},
			{{MXT_CAST(State::BW_START_HS), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_START);
				auto minTimeStartToHs = computeMinTime(mConfig->startToHs());
				minTimeStartToHs *= (1 - mConfig->timeTolerance());
				mTimeCtrl.setTimer(State::BW_START_HS, TimerEvent::ITEM_READY_HS, minTimeStartToHs);
			});
	//ITEM_READY_HS
	mLogic.transition(TimerEvent::ITEM_READY_HS,
			{{MXT_CAST(State::BW_START_HS), 1}},
			{{MXT_CAST(State::BF_LB_HS), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BW_START_HS);

				auto minTimeStartToHs = computeMinTime(mConfig->startToHs());
				minTimeStartToHs *= 1 - mConfig->timeTolerance();

				auto maxTimeStartToHs = computeMaxTime(mConfig->startToHs());
				maxTimeStartToHs *= 1 + mConfig->timeTolerance();
				mTimeCtrl.setTimer(State::BF_LB_HS, TimerEvent::START_2, maxTimeStartToHs-minTimeStartToHs);
			});
	//TIMER_START_2
	mLogic.transition(TimerEvent::START_2,
			{{MXT_CAST(State::BF_LB_HS), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_LB_HS);
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_HEIGHTSENSOR));
			});
	//LB_HS
	mLogic.transition(run::HalEvent::LB_HS,
			{{MXT_CAST(State::BF_LB_HS), 1}},
			{{MXT_CAST(State::IN_LB_HS), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_LB_HS);
				mTimeCtrl.setTimer(State::IN_LB_HS, TimerEvent::HS_1, computeMaxTime(mConfig->itemInLB()));
			});
	//LB_HS_E
	mLogic.transition(run::HalEvent::LB_HS,
			{{MXT_CAST(State::BF_LB_HS), 0}},
			{},
			[this](void)
			{
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_APPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_HEIGHTSENSOR));
			});



	//--------- Hoehenmessung bis zum Switch
	//TIMER_HS_1
	mLogic.transition(TimerEvent::HS_1,
			{{MXT_CAST(State::IN_LB_HS), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_HS);
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_STUCK, MXT_SHARE(data::Location, data::Location::Type::LB_HEIGHTSENSOR));
			});

	//!LB_HS
	mLogic.transition(run::HalEvent::I_LB_HS,
			{{MXT_CAST(State::IN_LB_HS), 1}},
			{{MXT_CAST(State::BW_HS_SWITCH), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_HS);
				mTimeCtrl.setTimer(State::BW_HS_SWITCH, TimerEvent::ITEM_READY_SWITCH, computeMinTime(mConfig->hsToSwitch()));
			});

	//ITEM_READY_SWITCH
	mLogic.transition(TimerEvent::ITEM_READY_SWITCH,
			{{MXT_CAST(State::BW_HS_SWITCH), 1}},
			{{MXT_CAST(State::BF_LB_SWITCH), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BW_HS_SWITCH);
				auto maxTimeDiff = computeMaxTime(1.2 * mConfig->hsToSwitch()) - computeMinTime(mConfig->hsToSwitch());
				mTimeCtrl.setTimer(State::BF_LB_SWITCH, TimerEvent::HS_2, maxTimeDiff);
			});

	//TIMER_HS_2
	mLogic.transition(TimerEvent::HS_2,
			{{MXT_CAST(State::BF_LB_SWITCH), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_LB_SWITCH);
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_SWITCH));
			});
	//LB_SWITCH
	mLogic.transition(run::HalEvent::LB_SWITCH,
			{{MXT_CAST(State::BF_LB_SWITCH), 1}},
			{{MXT_CAST(State::IN_SWITCH), 1},{MXT_CAST(State::IN_LB_SWITCH), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_LB_SWITCH);
				//check if there is measured date to send
				if(mHeightMapBuffer.empty() || !std::get<MXT_FINISHED>(mHeightMapBuffer.front()))
				{
					MXT_THROW_EX(NoMeasuredHighMap);
				}

				// send item info to master (heightmap, metalsensor)
				sendItemInfo(data::Data_ptr(std::get<MXT_HM>(mHeightMapBuffer.front())), MXT_SHARE(data::MetalSensor, HAL_METAL_SENSOR.isMetal()));
				// delete the old hightmap
				mHeightMapBuffer.pop_front();
				MXT_LOG_WARN("Set timer for itemInLB: ", computeMaxTime(mConfig->itemInLB()));
				mTimeCtrl.setTimer(State::IN_LB_SWITCH, TimerEvent::SWITCH_1, computeMaxTime(mConfig->itemInLB()));
			});
	//LB_SWITCH_E
	mLogic.transition(run::HalEvent::LB_SWITCH,
			{{MXT_CAST(State::BF_LB_SWITCH), 0}},
			{},
			[this](void)
			{
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_APPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_SWITCH));
			});



	//--------- Switch und Rampe
	//KEEP_NEXT
	mLogic.transition(Message::Run::KEEP_NEXT,
			{{MXT_CAST(State::IN_SWITCH), 1}},
			{{MXT_CAST(State::IN_SWITCH_KEEP), 1}},
			[this](void)
			{
				HAL_SWITCH.open();
				sendMasterMessage(Message::Run::KEEP_NEXT);
			});
	//TIMER_SWITCH_1
	mLogic.transition(TimerEvent::SWITCH_1,
			{{MXT_CAST(State::IN_LB_SWITCH), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_SWITCH);
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_STUCK, MXT_SHARE(data::Location, data::Location::Type::LB_SWITCH));
			});
	//!LB_SWITCH
	mLogic.transition(run::HalEvent::I_LB_SWITCH,
			{{MXT_CAST(State::IN_SWITCH), 1}, {MXT_CAST(State::IN_LB_SWITCH), 1}},
			{{MXT_CAST(State::BF_LB_RAMP), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_SWITCH);
			});
	//LB_RAMP
	mLogic.transition(run::HalEvent::LB_RAMP,
			{{MXT_CAST(State::BF_LB_RAMP), 1}},
			{{MXT_CAST(State::IN_LB_RAMP), 1}},
			[this](void)
			{
				mTimeCtrl.setTimer(State::IN_LB_RAMP, TimerEvent::RAMP, mConfig->rampTime());
			});
	//LB_RAMP_E
	mLogic.transition(run::HalEvent::LB_RAMP,
			{{MXT_CAST(State::BF_LB_RAMP), 0}},
			{},
			[this](void)
			{
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_APPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_RAMP));
			});
	//TIMER_RAMP
	mLogic.transition(TimerEvent::RAMP,
			{{MXT_CAST(State::IN_LB_RAMP), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_RAMP);
				sendMasterMessage(Message::Run::RAMP_FULL);
			});
	//!LB_RAMP
	mLogic.transition(run::HalEvent::I_LB_RAMP,
			{{MXT_CAST(State::IN_LB_RAMP), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::IN_LB_RAMP);
				sendMasterMessage(Message::Run::ITEM_REMOVED);
			});



	//--------- Switch bis zur Ausgabe
	//!LB_SWITCH_2
	mLogic.transition(run::HalEvent::I_LB_SWITCH,
			{{MXT_CAST(State::IN_SWITCH_KEEP), 1}, {MXT_CAST(State::IN_LB_SWITCH), 1}},
			{{MXT_CAST(State::BW_SWITCH_END), 1}, {MXT_CAST(State::SWITCH_CTRL), 1}},
			[this](void)
			{

				//TODO: need variable time for switch closing
				mTimeCtrl.setTimer(State::SWITCH_CTRL, TimerEvent::CLOSE_SWITCH, 400);
				mTimeCtrl.deleteTimer(State::IN_LB_SWITCH);
				mTimeCtrl.setTimer(State::BW_SWITCH_END, TimerEvent::ITEM_READY_END, computeMinTime(mConfig->switchToEnd()));
			});
	//CLOSE_SWITCH
	mLogic.transition(TimerEvent::CLOSE_SWITCH,
			{{MXT_CAST(State::SWITCH_CTRL), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::SWITCH_CTRL);
				HAL_SWITCH.close();
			});
	//ITEM_READY_END
	mLogic.transition(TimerEvent::ITEM_READY_END,
			{{MXT_CAST(State::BW_SWITCH_END), 1}},
			{{MXT_CAST(State::BF_END), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BW_SWITCH_END);
				auto maxTimeDiff = computeMaxTime(mConfig->switchToEnd()) - computeMinTime(mConfig->switchToEnd());
				mTimeCtrl.setTimer(State::BF_END, TimerEvent::SWITCH_3, maxTimeDiff);
			});
	//TIMER_SWITCH_3
	mLogic.transition(TimerEvent::SWITCH_3,
			{{MXT_CAST(State::BF_END), 1}},
			{},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_END);
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_END));
			});
	//LB_END
	mLogic.transition(run::HalEvent::LB_END,
			{{MXT_CAST(State::BF_END), 1}},
			{{MXT_CAST(State::IN_LB_END), 1}},
			[this](void)
			{
				mTimeCtrl.deleteTimer(State::BF_END);
				sendMasterMessage(Message::Run::REACHED_END);
			});

	//LB_END_E
	mLogic.transition(run::HalEvent::LB_END,
			{{MXT_CAST(State::BF_END), 0}},
			{},
			[this](void)
			{
				sendMessageWithData(Location::MASTER, Message::Run::ITEM_APPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_END));
			});
	//!LB_END
	mLogic.transition(run::HalEvent::I_LB_END,
			{{MXT_CAST(State::IN_LB_END), 1}},
			{},
			[this](void)
			{
				sendMasterMessage(Message::Run::END_FREE);
			});
}

uint32_t RunManager::computeMinTime(uint32_t time)
{
	return time * (1 - mConfig->tolerance());
}

uint32_t RunManager::computeMaxTime(uint32_t time)
{
	return time * (1 + MXT_TOLERANCE_MULT * mConfig->tolerance());
}

}}


