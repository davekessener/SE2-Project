#include "base/run_manager.h"
#include "lib/timer.h"
#include "lib/timer/manager.h"
#include "communication/packet.h"
#include "communication/message.h"
#include "system.h"
#include "types.h"


namespace esep { namespace base {

#define MXT_TIME_FOR_EXPCT_NEW	3000
#define MXT_TIME_IN_LB			3000
#define MXT_CAST(t)				static_cast<uint8_t>(t)
#define MXT_CONFIG				this->mConfigData

void RunManager::initLogic()
{
	//--------- Eingabe bis zur Hoehenmessung
	//EXPECT_NEW
	mLogic.transition(runMessage_t::EXPECT_NEW,
			{},
			{{MXT_CAST(State::STATE_1), 1}},
			[this](void)
			{
				this->mTimeCtrl.setTimer(State::STATE_1, run::TimerEvent::EXPECT_NEW, MXT_TIME_FOR_EXPCT_NEW);
			});
	//TIMER_EXPECT_NEW
	mLogic.transition(run::TimerEvent::EXPECT_NEW,
			{{MXT_CAST(State::STATE_1), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::HAND_OVER);
			});
	//LB_START_1
	mLogic.transition(run::HalEvent::LB_START,
			{{MXT_CAST(State::STATE_1), 1}},
			{{MXT_CAST(State::STATE_2), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_1);
				this->mTimeCtrl.setTimer(State::STATE_2, run::TimerEvent::START_1, MXT_TIME_IN_LB);
			});
	//LB_START_2
	mLogic.transition(run::HalEvent::LB_START,
			{{MXT_CAST(State::STATE_1), 0}},
			{{MXT_CAST(State::STATE_2), 1}},
			[this](void)
			{
				this->mTimeCtrl.setTimer(State::STATE_2, run::TimerEvent::START_1, MXT_TIME_IN_LB);
			});
	//TIMER_START_1
	mLogic.transition(run::TimerEvent::START_1,
			{{MXT_CAST(State::STATE_2), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::LB_START);
			});
	//!LB_START
	mLogic.transition(run::HalEvent::I_LB_START,
			{{MXT_CAST(State::STATE_2), 1}},
			{{MXT_CAST(State::STATE_3), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_2);
				auto minTimeStartToHs = computeMinTime(MXT_CONFIG->startToHs());
				minTimeStartToHs =- minTimeStartToHs * MXT_CONFIG->timeTolerance();
				this->mTimeCtrl.setTimer(State::STATE_3, run::TimerEvent::ITEM_READY_HS, minTimeStartToHs);
			});
	//ITEM_READY_HS
	mLogic.transition(run::TimerEvent::ITEM_READY_HS,
			{{MXT_CAST(State::STATE_3), 1}},
			{{MXT_CAST(State::STATE_4), 1}},
			[this](void)
			{
				auto minTimeStartToHs = computeMinTime(MXT_CONFIG->startToHs());
				minTimeStartToHs =- minTimeStartToHs * MXT_CONFIG->timeTolerance();

				auto maxTimeStartToHs = computeMaxTime(MXT_CONFIG->startToHs());
				maxTimeStartToHs =+ maxTimeStartToHs * MXT_CONFIG->timeTolerance();
				this->mTimeCtrl.setTimer(State::STATE_4, run::TimerEvent::START_2, maxTimeStartToHs-minTimeStartToHs);
			});
	//TIMER_START_2
	mLogic.transition(run::TimerEvent::START_2,
			{{MXT_CAST(State::STATE_4), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::LB_START);
			});
	//LB_HS
	mLogic.transition(run::HalEvent::LB_HS,
			{{MXT_CAST(State::STATE_4), 1}},
			{{MXT_CAST(State::STATE_5), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_4);
				this->mTimeCtrl.setTimer(State::STATE_5, run::TimerEvent::HS_1, MXT_TIME_IN_LB);
			});
	//LB_HS_E
	mLogic.transition(run::HalEvent::LB_HS,
			{{MXT_CAST(State::STATE_4), 0}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_APPEARED, data::Location::Type::LB_HEIGHTSENSOR);
			});

	//--------- Hoehenmessung bis zum Switch
	//TIMER_HS_1
	mLogic.transition(run::TimerEvent::HS_1,
			{{MXT_CAST(State::STATE_5), 1}},
			{},
			[this](void)
			{
				this->sendErrorMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::LB_HEIGHTSENSOR);
			});

	//!LB_HS
	mLogic.transition(run::HalEvent::I_LB_HS,
			{{MXT_CAST(State::STATE_5), 1}},
			{{MXT_CAST(State::STATE_6), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_5);
				this->mTimeCtrl.setTimer(State::STATE_6, run::TimerEvent::ITEM_READY_SWITCH, computeMinTime(MXT_CONFIG->hsToSwitch()));
			});

	//ITEM_READY_SWITCH
	mLogic.transition(run::TimerEvent::ITEM_READY_SWITCH,
			{{MXT_CAST(State::STATE_6), 1}},
			{{MXT_CAST(State::STATE_7), 1}},
			[this](void)
			{
				auto maxTimeDiff = computeMaxTime(MXT_CONFIG->hsToSwitch()) - computeMinTime(MXT_CONFIG->hsToSwitch());
				this->mTimeCtrl.setTimer(State::STATE_7, run::TimerEvent::HS_2, maxTimeDiff);
			});

	//TIMER_HS_2
	mLogic.transition(run::TimerEvent::HS_2,
			{{MXT_CAST(State::STATE_7), 1}},
			{},
			[this](void)
			{
				this->sendErrorMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::LB_HEIGHTSENSOR);
			});
	//LB_SWITCH
	mLogic.transition(run::HalEvent::LB_SWITCH,
			{{MXT_CAST(State::STATE_7), 1}},
			{{MXT_CAST(State::STATE_8), 1}},
			[this](void)
			{
				this->mTimeCtrl.setTimer(State::STATE_8, run::TimerEvent::SWITCH_1, MXT_TIME_IN_LB);
			});
	//LB_SWITCH_E
	mLogic.transition(run::HalEvent::LB_SWITCH,
			{{MXT_CAST(State::STATE_7), 0}},
			{},
			[this](void)
			{
				this->sendErrorMessage(runMessage_t::ITEM_APPEARED, data::Location::Type::LB_SWITCH);
			});

	//--------- Switch und Rampe
	//KEEP_NEXT
	mLogic.transition(runMessage_t::KEEP_NEXT,
			{{MXT_CAST(State::STATE_8), 1}},
			{{MXT_CAST(State::STATE_11), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_8);
				this->mTimeCtrl.setTimer(State::STATE_11, run::TimerEvent::SWITCH_2, MXT_TIME_IN_LB);
			});
	//TIMER_SWITCH_1
	mLogic.transition(run::TimerEvent::SWITCH_1,
			{{MXT_CAST(State::STATE_8), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::LB_SWITCH);
			});
	//!LB_SWITCH
	mLogic.transition(run::HalEvent::I_LB_SWITCH,
			{{MXT_CAST(State::STATE_8), 1}},
			{{MXT_CAST(State::STATE_9), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_8);
			});
	//LB_RAMP
	mLogic.transition(run::HalEvent::LB_RAMP,
			{{MXT_CAST(State::STATE_9), 1}},
			{{MXT_CAST(State::STATE_10), 1}},
			[this](void)
			{
				this->mTimeCtrl.setTimer(State::STATE_10, run::TimerEvent::RAMP, MXT_TIME_IN_LB);
			});
	//LB_RAMP_E
	mLogic.transition(run::HalEvent::LB_RAMP,
			{{MXT_CAST(State::STATE_9), 0}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_APPEARED, data::Location::Type::LB_RAMP);
			});
	//TIMER_RAMP
	mLogic.transition(run::TimerEvent::RAMP,
			{{MXT_CAST(State::STATE_10), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::RAMP_FULL, data::Location::Type::LB_RAMP);
			});
	//TIMER_RAMP
	mLogic.transition(run::HalEvent::I_LB_RAMP,
			{{MXT_CAST(State::STATE_10), 1}},
			{},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_10);
			});

	//--------- Switch bis zur Ausgabe
	//TIMER_SWITCH_2
	mLogic.transition(run::TimerEvent::SWITCH_2,
			{{MXT_CAST(State::STATE_11), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::LB_SWITCH);
			});
	//!LB_SWITCH_2
	mLogic.transition(run::TimerEvent::SWITCH_2,
			{{MXT_CAST(State::STATE_11), 1}},
			{{MXT_CAST(State::STATE_12), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_11);
				this->mTimeCtrl.setTimer(State::STATE_12, run::TimerEvent::ITEM_READY_END, computeMinTime(MXT_CONFIG->switchToEnd()));
			});
	//ITEM_READY_END
	mLogic.transition(run::TimerEvent::ITEM_READY_END,
			{{MXT_CAST(State::STATE_12), 1}},
			{{MXT_CAST(State::STATE_13), 1}},
			[this](void)
			{
				auto maxTimeDiff = computeMaxTime(MXT_CONFIG->switchToEnd()) - computeMinTime(MXT_CONFIG->switchToEnd());
				this->mTimeCtrl.setTimer(State::STATE_13, run::TimerEvent::SWITCH_3, maxTimeDiff);
			});
	//TIMER_SWITCH_3
	mLogic.transition(run::TimerEvent::SWITCH_3,
			{{MXT_CAST(State::STATE_13), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::ITEM_DISAPPEARED, data::Location::Type::LB_SWITCH);
			});
	//LB_END
	mLogic.transition(run::HalEvent::LB_END,
			{{MXT_CAST(State::STATE_13), 1}},
			{{MXT_CAST(State::STATE_14), 1}},
			[this](void)
			{
				this->mTimeCtrl.deleteTimer(State::STATE_13);
				this->sendMasterMessage(runMessage_t::REACHED_END);
			});
	//!LB_END
	mLogic.transition(run::HalEvent::I_LB_END,
			{{MXT_CAST(State::STATE_14), 1}},
			{},
			[this](void)
			{
				this->sendMasterMessage(runMessage_t::END_FREE);
			});
}

uint32_t RunManager::computeMinTime(uint32_t time)
{
	return time-(time*mConfigData->TOLERANCE);
}

uint32_t RunManager::computeMaxTime(uint32_t time)
{
	return time+(time*mConfigData->TOLERANCE);
}

}}


