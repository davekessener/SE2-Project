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
#define MXT_CREATE_PACKET(m) 	std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, m)
#define MXT_CAST(t)				static_cast<uint8_t>(t)

void RunManager::initLogic()
{
	auto noFunc = [](void){};

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
				auto p = MXT_CREATE_PACKET(runMessage_t::ITEM_DISAPPEARED);
				//p->addDataPoint(std::make_shared<data::DataPoint>(data::Location()))
				this->mMaster->accept(p);
			});
	//LB_START_1
	mLogic.transition(run::HalEvent::LB_START,
			{{MXT_CAST(State::STATE_1), 1}},
			{{MXT_CAST(State::STATE_2), 1}},
			[this](void)
			{
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
	mLogic.transition(run::TimerEvent::EXPECT_NEW,
			{{MXT_CAST(State::STATE_1), 1}},
			{},
			[this](void)
			{
				this->mMaster->accept(MXT_CREATE_PACKET(runMessage_t::ITEM_DISAPPEARED));
			});
}

}}


