#include "base/run_manager.h"
#include "lib/timer.h"
#include "lib/timer/manager.h"
#include "communication/packet.h"
#include "communication/message.h"
#include "system.h"

namespace esep { namespace base {

#define MXT_TIME_IN_LB			3000
#define MXT_CREATE_PACKET(m) 	std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, m)

void RunManager::initLogic()
{
	auto f = [](void){};

	mLogic.transition(Message::EXPECT_NEW,
			{},
			{{1, 1}},
			[this](void)
			{
				HAL_MOTOR.right();
				HAL_MOTOR.start();
				this->mTimeCtrl.setTimer(itemID, run::TimerEvent::EXPECT_NEW, MXT_TIME_IN_LB);
			});

	mLogic.transition(run::TimerEvent::EXPECT_NEW,
			{{1,1}},
			{},
			[this](void)
			{
				this->mMaster->accept(MXT_CREATE_PACKET(Message::ITEM_DISAPPEARED));
			});

	mLogic.transition(Event::LB_START,
			{{1,0}},
			{{2,1}},
			[this](void)
			{
				HAL_MOTOR.right();
				HAL_MOTOR.start();
			});

	mLogic.transition(Event::LB_START,
			{{1,1}},
			{{1,1}},
			f);
}

}}


