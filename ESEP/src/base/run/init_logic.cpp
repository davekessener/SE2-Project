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

#define MXT_TIME_FOR_EXPCT_NEW	3000
#define MXT_TIME_IN_LB			3000
#define MXT_CAST(t)				static_cast<uint8_t>(t)
#define MXT_SHARE(T, V)			data::Data_ptr(new T(V))
#define MXT_HM					2
#define MXT_FINISHED			0

void RunManager::initLogic()
{
	//--------- Eingabe bis zur Hoehenmessung
	//EXPECT_NEW
	mLogic.transition(Message::Run::EXPECT_NEW,
			{},
			{{MXT_CAST(State::STATE_1), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: EXPECT_NEW");
				this->mTimeCtrl.setTimer(State::STATE_1, TimerEvent::EXPECT_NEW, mConfig->maxHandOverTime());
			});
	//TIMER_EXPECT_NEW
	mLogic.transition(TimerEvent::EXPECT_NEW,
			{{MXT_CAST(State::STATE_1), 1}},
			{},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_1);
				this->sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_START));
			});
	//LB_START_1
	mLogic.transition(run::HalEvent::LB_START,
			{{MXT_CAST(State::STATE_1), 1}},
			{{MXT_CAST(State::STATE_2), 1}},
			[this](void)
			{
				this->sendMasterMessage(Message::Run::NEW_ITEM);
				this->mTimeCtrl.deleteTimer(State::STATE_1);
				//this->mTimeCtrl.setTimer(State::STATE_2, TimerEvent::START_1, MXT_TIME_IN_LB);
			});
	//LB_START_2
	mLogic.transition(run::HalEvent::LB_START,
			{{MXT_CAST(State::STATE_1), 0}},
			{{MXT_CAST(State::STATE_2), 1}},
			[this](void)
			{
				this->sendMasterMessage(Message::Run::NEW_ITEM);
				//this->mTimeCtrl.setTimer(State::STATE_2, TimerEvent::START_1, MXT_TIME_IN_LB);
			});
	//TIMER_START_1
	mLogic.transition(TimerEvent::START_1,
			{{MXT_CAST(State::STATE_2), 1}},
			{},
			[this](void)
			{
				//this->sendMessage(Location::MASTER, runMessage_t::ITEM_DISAPPEARED, MXT_SHARE(data::Location::Type::LB_START));
			});
	//!LB_START
	mLogic.transition(run::HalEvent::I_LB_START,
			{{MXT_CAST(State::STATE_2), 1}},
			{{MXT_CAST(State::STATE_3), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: ITEM_READY_HS");
				//this->mTimeCtrl.deleteTimer(State::STATE_2);
				auto minTimeStartToHs = computeMinTime(mConfig->startToHs());
				minTimeStartToHs *= (1 - mConfig->timeTolerance());
				this->mTimeCtrl.setTimer(State::STATE_3, TimerEvent::ITEM_READY_HS, minTimeStartToHs);
			});
	//ITEM_READY_HS
	mLogic.transition(TimerEvent::ITEM_READY_HS,
			{{MXT_CAST(State::STATE_3), 1}},
			{{MXT_CAST(State::STATE_4), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: START_2");
				this->mTimeCtrl.deleteTimer(State::STATE_3);

				auto minTimeStartToHs = computeMinTime(mConfig->startToHs());
				minTimeStartToHs *= 1 - mConfig->timeTolerance();

				auto maxTimeStartToHs = computeMaxTime(mConfig->startToHs());
				maxTimeStartToHs *= 1 + mConfig->timeTolerance();
				this->mTimeCtrl.setTimer(State::STATE_4, TimerEvent::START_2, maxTimeStartToHs-minTimeStartToHs);
			});
	//TIMER_START_2
	mLogic.transition(TimerEvent::START_2,
			{{MXT_CAST(State::STATE_4), 1}},
			{},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_4);
				this->sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_HEIGHTSENSOR));
			});
	//LB_HS
	mLogic.transition(run::HalEvent::LB_HS,
			{{MXT_CAST(State::STATE_4), 1}},
			{{MXT_CAST(State::STATE_5), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_4);
				//this->mTimeCtrl.setTimer(State::STATE_5, TimerEvent::HS_1, MXT_TIME_IN_LB);
			});
	//LB_HS_E
	mLogic.transition(run::HalEvent::LB_HS,
			{{MXT_CAST(State::STATE_4), 0}},
			{},
			[this](void)
			{
				this->sendMessageWithData(Location::MASTER, Message::Run::ITEM_APPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_HEIGHTSENSOR));
			});



	//--------- Hoehenmessung bis zum Switch
	//TIMER_HS_1
	mLogic.transition(TimerEvent::HS_1,
			{{MXT_CAST(State::STATE_5), 1}},
			{},
			[this](void)
			{
				//this->sendMessage(Location::MASTER, runMessage_t::ITEM_DISAPPEARED, MXT_SHARE(data::Location::Type::LB_HEIGHTSENSOR));
			});

	//!LB_HS
	mLogic.transition(run::HalEvent::I_LB_HS,
			{{MXT_CAST(State::STATE_5), 1}},
			{{MXT_CAST(State::STATE_6), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: ITEM_READY_SWITCH");
				//this->mTimeCtrl.deleteTimer(State::STATE_5);
				this->mTimeCtrl.setTimer(State::STATE_6, TimerEvent::ITEM_READY_SWITCH, computeMinTime(mConfig->hsToSwitch()));
			});

	//ITEM_READY_SWITCH
	mLogic.transition(TimerEvent::ITEM_READY_SWITCH,
			{{MXT_CAST(State::STATE_6), 1}},
			{{MXT_CAST(State::STATE_7), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: HS_2");
				this->mTimeCtrl.deleteTimer(State::STATE_6);
				auto maxTimeDiff = computeMaxTime(mConfig->hsToSwitch()) - computeMinTime(mConfig->hsToSwitch());
				this->mTimeCtrl.setTimer(State::STATE_7, TimerEvent::HS_2, maxTimeDiff);
			});

	//TIMER_HS_2
	mLogic.transition(TimerEvent::HS_2,
			{{MXT_CAST(State::STATE_7), 1}},
			{},
			[this](void)
			{
				MXT_LOG_WARN("RUN: deleted timer event in state 7");
				this->mTimeCtrl.deleteTimer(State::STATE_7);
				MXT_LOG_WARN("RUN: deleted timer event 6");
				this->sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_SWITCH));
			});
	//LB_SWITCH
	mLogic.transition(run::HalEvent::LB_SWITCH,
			{{MXT_CAST(State::STATE_7), 1}},
			{{MXT_CAST(State::STATE_8), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: deleted timer event in state 7");
				this->mTimeCtrl.deleteTimer(State::STATE_7);
				MXT_LOG_WARN("RUN: deleted timer event 6");
				//check if there is measured date to send
				if(mHeightMapBuffer.empty() || !std::get<MXT_FINISHED>(mHeightMapBuffer.front()))
				{
					MXT_THROW_EX(NoMeasuredHighMap);
				}

				// send item info to master (heightmap, metalsensor)
				this->sendItemInfo(data::Data_ptr(std::get<MXT_HM>(mHeightMapBuffer.front())), MXT_SHARE(data::MetalSensor, HAL_METAL_SENSOR.isMetal()));
				// delete the old hightmap
				mHeightMapBuffer.pop_front();
				//this->mTimeCtrl.setTimer(State::STATE_8, TimerEvent::SWITCH_1, MXT_TIME_IN_LB);
			});
	//LB_SWITCH_E
	mLogic.transition(run::HalEvent::LB_SWITCH,
			{{MXT_CAST(State::STATE_7), 0}},
			{},
			[this](void)
			{
				this->sendMessageWithData(Location::MASTER, Message::Run::ITEM_APPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_SWITCH));
			});



	//--------- Switch und Rampe
	//KEEP_NEXT
	mLogic.transition(Message::Run::KEEP_NEXT,
			{{MXT_CAST(State::STATE_8), 1}},
			{{MXT_CAST(State::STATE_11), 1}},
			[this](void)
			{
				HAL_SWITCH.open();
				//this->mTimeCtrl.deleteTimer(State::STATE_8);
				//this->mTimeCtrl.setTimer(State::STATE_11, TimerEvent::SWITCH_2, MXT_TIME_IN_LB);
			});
	//TIMER_SWITCH_1
	mLogic.transition(TimerEvent::SWITCH_1,
			{{MXT_CAST(State::STATE_8), 1}},
			{},
			[this](void)
			{
				//this->sendMessage(Location::MASTER, runMessage_t::ITEM_DISAPPEARED, MXT_SHARE(data::Location::Type::LB_SWITCH));
			});
	//!LB_SWITCH
	mLogic.transition(run::HalEvent::I_LB_SWITCH,
			{{MXT_CAST(State::STATE_8), 1}},
			{{MXT_CAST(State::STATE_9), 1}},
			[this](void)
			{
				//this->mTimeCtrl.deleteTimer(State::STATE_8);
			});
	//LB_RAMP
	mLogic.transition(run::HalEvent::LB_RAMP,
			{{MXT_CAST(State::STATE_9), 1}},
			{{MXT_CAST(State::STATE_10), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: RAMP");
				//TODO i need a new time, for the ramp to trigger a full ramp
				this->mTimeCtrl.setTimer(State::STATE_10, TimerEvent::RAMP, mConfig->maxHandOverTime());
			});
	//LB_RAMP_E
	mLogic.transition(run::HalEvent::LB_RAMP,
			{{MXT_CAST(State::STATE_9), 0}},
			{},
			[this](void)
			{
				this->sendMessageWithData(Location::MASTER, Message::Run::ITEM_APPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_RAMP));
			});
	//TIMER_RAMP
	mLogic.transition(TimerEvent::RAMP,
			{{MXT_CAST(State::STATE_10), 1}},
			{},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_10);
				this->sendMasterMessage(Message::Run::RAMP_FULL);
			});
	//!LB_RAMP
	mLogic.transition(run::HalEvent::I_LB_RAMP,
			{{MXT_CAST(State::STATE_10), 1}},
			{},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_10);
				//send item is successfully removed
				this->sendMasterMessage(Message::Run::ITEM_REMOVED);
			});



	//--------- Switch bis zur Ausgabe
	//TIMER_SWITCH_2
	mLogic.transition(TimerEvent::SWITCH_2,
			{{MXT_CAST(State::STATE_11), 1}},
			{},
			[this](void)
			{
				//this->sendMessage(Location::MASTER, runMessage_t::ITEM_DISAPPEARED, MXT_SHARE(data::Location::Type::LB_SWITCH));
			});
	//!LB_SWITCH_2
	mLogic.transition(run::HalEvent::I_LB_SWITCH,
			{{MXT_CAST(State::STATE_11), 1}},
			{{MXT_CAST(State::STATE_12), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: ITEM_READY_END");
				HAL_SWITCH.close();
				//this->mTimeCtrl.deleteTimer(State::STATE_11);
				this->mTimeCtrl.setTimer(State::STATE_12, TimerEvent::ITEM_READY_END, computeMinTime(mConfig->switchToEnd()));
			});
	//ITEM_READY_END
	mLogic.transition(TimerEvent::ITEM_READY_END,
			{{MXT_CAST(State::STATE_12), 1}},
			{{MXT_CAST(State::STATE_13), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: set timer: SWITCH_3");
				this->mTimeCtrl.deleteTimer(State::STATE_12);
				auto maxTimeDiff = computeMaxTime(mConfig->switchToEnd()) - computeMinTime(mConfig->switchToEnd());
				this->mTimeCtrl.setTimer(State::STATE_13, TimerEvent::SWITCH_3, maxTimeDiff);
			});
	//TIMER_SWITCH_3
	mLogic.transition(TimerEvent::SWITCH_3,
			{{MXT_CAST(State::STATE_13), 1}},
			{},
			[this](void)
			{
				MXT_LOG_WARN("RUN: deleted timer: SWITCH_3");
				this->mTimeCtrl.deleteTimer(State::STATE_13);
				this->sendMessageWithData(Location::MASTER, Message::Run::ITEM_DISAPPEARED, MXT_SHARE(data::Location, data::Location::Type::LB_END));
			});
	//LB_END
	mLogic.transition(run::HalEvent::LB_END,
			{{MXT_CAST(State::STATE_13), 1}},
			{{MXT_CAST(State::STATE_14), 1}},
			[this](void)
			{
				MXT_LOG_WARN("RUN: deleted timer: SWITCH_3");
				this->mTimeCtrl.deleteTimer(State::STATE_13);
				this->sendMasterMessage(Message::Run::REACHED_END);
			});
	//!LB_END
	mLogic.transition(run::HalEvent::I_LB_END,
			{{MXT_CAST(State::STATE_14), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(Message::Run::END_FREE);
			});
}

uint32_t RunManager::computeMinTime(uint32_t time)
{
	return time * (1 - mConfig->TOLERANCE);
}

uint32_t RunManager::computeMaxTime(uint32_t time)
{
	return time * (1 + mConfig->TOLERANCE);
}

}}


