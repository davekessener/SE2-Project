
#include <iostream>
#include "test/ut/run_manager_logic.h"

#include "test/unit/assertions.h"
#include "test/unit/hal.h"


namespace esep { namespace test { namespace ut {

#define MXT_SLEEP(t)		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(t)))
#define MXT_BM_SWITCH 		(1u << 19)
#define MXT_BM_MOTOR_START	(1u << 12)


struct BasicRecipient : public communication::IRecipient
{
	typedef communication::Packet_ptr Packet_ptr;

	void accept(Packet_ptr p) override
	{
		mPackets.push_back(p);
	}
	std::deque<Packet_ptr> mPackets;
};

RunManagerLogic::RunManagerLogic()
	: TestSuite("Run Manager")
{
	  mRunManager =nullptr;
	  mCom = nullptr;
	  mConfig = nullptr;
}

void RunManagerLogic::setup(void)
{
	mCom = new BasicRecipient;
	mConfig = new config_t("ut.conf");
	mConfig->setStartToHs(10);
	mConfig->setHsToSwitch(15);
	mConfig->setSwitchToEnd(20);
	mConfig->setMaxHandOverTime(5);
	mConfig->setHeightSensorMax(10);
	mConfig->setHeightSensorMin(10);
	mConfig->setSlowFactor(1);
	mConfig->setTimeTolerance(1);
	mRunManager = new base::RunManager(mCom, mConfig);
	hal().setCallback( [this](Event e) { mRunManager->handle(e); } );
}

void RunManagerLogic::teardown(void)
{
	delete mConfig; mConfig = nullptr;
	delete mCom; mCom = nullptr;
	delete mRunManager; mRunManager = nullptr;
}

void RunManagerLogic::sendPacket(msg_t msg)
{
	mRunManager->accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, msg));
}

uint32_t RunManagerLogic::maxTime(uint32_t t)
{
	return t *= (1 + mConfig->TOLERANCE);
}

void RunManagerLogic::blockLB(LightBarrier lb)
{
	hal().setField(Field::GPIO_0, hal().getField(Field::GPIO_0) & ~static_cast<uint32_t>(lb));
}

void RunManagerLogic::freeLB(LightBarrier lb)
{
	hal().setField(Field::GPIO_0, hal().getField(Field::GPIO_0) | static_cast<uint32_t>(lb));
}

void RunManagerLogic::define(void)
{
//	UNIT_TEST("Can Create RunManager")
//	{
//
//	};
//
//	UNIT_TEST("resume and suspend")
//	{
//		sendPacket(Message::Run::RESUME);
//		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
//		ASSERT_TRUE(hal().writes().back().get<uint32_t>() & MXT_BM_MOTOR_START);
//
//		sendPacket(Message::Run::SUSPEND);
//		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
//		ASSERT_FALSE(hal().writes().back().get<uint32_t>() & MXT_BM_MOTOR_START); //right comparrison?
//	};

	UNIT_TEST("positive test - from start to end")
	{
		//Expect new -> LB_HS
		mRunManager->enter();
		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		std::cout << "mCom:" << mCom << std::endl;
		if(!mCom->mPackets.empty())
		{

		}

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::NEW_ITEM);
		mCom->mPackets.pop_front();

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		//LB_HS -> LB_SWITCH

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::ANALYSE);
		mCom->mPackets.pop_front();

		sendPacket(Message::Run::KEEP_NEXT);

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_0);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_SWITCH, MXT_BM_SWITCH);

		//LB_SWITCH -> LB_END

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(mConfig->switchToEnd());

		blockLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::REACHED_END);
		mCom->mPackets.pop_front();

		freeLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::END_FREE);
		mCom->mPackets.pop_front();
	};

//	UNIT_TEST("positive test - from start to ramp")
//	{
//		mRunManager->enter();
//		sendPacket(Message::Run::EXPECT_NEW);
//		blockLB(LightBarrier::LB_START);
//		hal().trigger(Event::LB_START);
//
//		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
//		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::NEW_ITEM);
//		mCom->mPackets.pop_front();
//
//		freeLB(LightBarrier::LB_START);
//		hal().trigger(Event::LB_START);
//
//		MXT_SLEEP(mConfig->startToHs());
//
//		blockLB(LightBarrier::LB_HEIGHTSENSOR);
//		hal().trigger(Event::LB_HEIGHTSENSOR);
//
//		freeLB(LightBarrier::LB_HEIGHTSENSOR);
//		hal().trigger(Event::LB_HEIGHTSENSOR);
//
//		MXT_SLEEP(mConfig->hsToSwitch());
//
//		blockLB(LightBarrier::LB_SWITCH);
//		hal().trigger(Event::LB_SWITCH);
//
//		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
//		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::ANALYSE);
//		mCom->mPackets.pop_front();
//
//		//til now same as above
//
//		freeLB(LightBarrier::LB_SWITCH);
//		hal().trigger(Event::LB_SWITCH);
//
//		blockLB(LightBarrier::LB_RAMP);
//		hal().trigger(Event::LB_RAMP);
//
//		freeLB(LightBarrier::LB_RAMP);
//		hal().trigger(Event::LB_RAMP);
//
//		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
//		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::ITEM_REMOVED);
//		mCom->mPackets.pop_front();
//	};
//
//	UNIT_TEST("ramp full")
//	{
//		mRunManager->enter();
//		sendPacket(Message::Run::EXPECT_NEW);
//		blockLB(LightBarrier::LB_START);
//		hal().trigger(Event::LB_START);
//
//		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
//		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::NEW_ITEM);
//		mCom->mPackets.pop_front();
//
//		freeLB(LightBarrier::LB_START);
//		hal().trigger(Event::LB_START);
//
//		MXT_SLEEP(mConfig->startToHs());
//
//		blockLB(LightBarrier::LB_HEIGHTSENSOR);
//		hal().trigger(Event::LB_HEIGHTSENSOR);
//
//		freeLB(LightBarrier::LB_HEIGHTSENSOR);
//		hal().trigger(Event::LB_HEIGHTSENSOR);
//
//		MXT_SLEEP(mConfig->hsToSwitch());
//
//		blockLB(LightBarrier::LB_SWITCH);
//		hal().trigger(Event::LB_SWITCH);
//
//		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
//		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::ANALYSE);
//		mCom->mPackets.pop_front();
//
//		freeLB(LightBarrier::LB_SWITCH);
//		hal().trigger(Event::LB_SWITCH);
//
//		blockLB(LightBarrier::LB_RAMP);
//		hal().trigger(Event::LB_RAMP);
//
//		//til now same as above
//
//		MXT_SLEEP(mConfig->maxHandOverTime());
//
//		ASSERT_EQUALS(mCom->mPackets.back()->message(), Message::Run::RAMP_FULL);
//		mCom->mPackets.pop_front();
//	};
//
//	UNIT_TEST("Item disappeared by handing over")
//	{
//		mRunManager->enter();
//		sendPacket(Message::Run::EXPECT_NEW);
//		ASSERT_TRUE(mCom->mPackets.empty());
//
//		MXT_SLEEP(mConfig->maxHandOverTime());
//		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
//		ASSERT_EQUALS(mCom->mPackets.front()->message(), Message::Run::ITEM_DISAPPEARED);
//
//		MXT_SLEEP(mConfig->maxHandOverTime());
//		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
//	};

}

}}}
