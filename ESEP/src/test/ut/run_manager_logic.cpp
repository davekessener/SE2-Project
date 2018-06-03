#include "test/ut/run_manager_logic.h"

#include "test/unit/assertions.h"
#include "test/unit/hal.h"

namespace esep { namespace test { namespace ut {

#define MXT_SLEEP(t)	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(t)))
#define MXT_BM_SWITCH 	(1u<<19)


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
	  mConfig = new config_t("ut.conf");
	  mConfig->setStartToHs(10);
	  mConfig->setHsToSwitch(15);
	  mConfig->setSwitchToEnd(20);
	  mConfig->setMaxHandOverTime(5);
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

void RunManagerLogic::define(void)
{
	UNIT_TEST("Can Create RunManager")
	{

	};

	UNIT_TEST("positive test - from start to end")
	{
		//Expect new -> LB_HS
		mRunManager->enter();
		sendPacket(runMessage_t::EXPECT_NEW);
		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_START));
		hal().trigger(Event::LB_START);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::NEW_ITEM);
		mCom->mPackets.pop_front();

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_START));
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_HEIGHTSENSOR));
		hal().trigger(Event::LB_HEIGHTSENSOR);

		//LB_HS -> LB_SWITCH

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_HEIGHTSENSOR));
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_SWITCH));
		hal().trigger(Event::LB_SWITCH);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::ANALYSE);
		mCom->mPackets.pop_front();

		sendPacket(runMessage_t::KEEP_NEXT);

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_0);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>(), MXT_BM_SWITCH);

		//LB_SWITCH -> LB_END

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_SWITCH));
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(mConfig->switchToEnd());

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_END));
		hal().trigger(Event::LB_END);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::REACHED_END);
		mCom->mPackets.pop_front();

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_END));
		hal().trigger(Event::LB_END);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::END_FREE);
		mCom->mPackets.pop_front();
	};

	UNIT_TEST("positive test - from start to ramp")
	{
		mRunManager->enter();
		sendPacket(runMessage_t::EXPECT_NEW);
		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_START));
		hal().trigger(Event::LB_START);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::NEW_ITEM);
		mCom->mPackets.pop_front();

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_START));
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_HEIGHTSENSOR));
		hal().trigger(Event::LB_HEIGHTSENSOR);

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_HEIGHTSENSOR));
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_SWITCH));
		hal().trigger(Event::LB_SWITCH);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::ANALYSE);
		mCom->mPackets.pop_front();

		//til now same as above

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_SWITCH));
		hal().trigger(Event::LB_SWITCH);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_RAMP));
		hal().trigger(Event::LB_RAMP);

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_RAMP));
		hal().trigger(Event::LB_RAMP);

		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::ITEM_REMOVED);
		mCom->mPackets.pop_front();
	};

	UNIT_TEST("ramp full")
	{
		mRunManager->enter();
		sendPacket(runMessage_t::EXPECT_NEW);
		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_START));
		hal().trigger(Event::LB_START);

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_START));
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_HEIGHTSENSOR));
		hal().trigger(Event::LB_HEIGHTSENSOR);

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_HEIGHTSENSOR));
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_SWITCH));
		hal().trigger(Event::LB_SWITCH);

		hal().setField(Field::GPIO_0, ~static_cast<uint32_t>(LightBarrier::LB_SWITCH));
		hal().trigger(Event::LB_SWITCH);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_RAMP));
		hal().trigger(Event::LB_RAMP);

		//til now same as above

		MXT_SLEEP(mConfig->maxHandOverTime());

		ASSERT_TRUE(mCom->mPackets.back()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.back()->message().as<runMessage_t>(), runMessage_t::RAMP_FULL);
		mCom->mPackets.pop_front();
	};

	UNIT_TEST("Item disappeared by handing over")
	{
		mRunManager->enter();
		sendPacket(runMessage_t::EXPECT_NEW);
		ASSERT_TRUE(mCom->mPackets.empty());

		MXT_SLEEP(maxTime(mConfig->maxHandOverTime()));
		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_TRUE(mCom->mPackets.front()->message().is<runMessage_t>());
		ASSERT_EQUALS(mCom->mPackets.front()->message().as<runMessage_t>(), runMessage_t::ITEM_DISAPPEARED);

		MXT_SLEEP(mConfig->maxHandOverTime());
		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
	};

}

}}}
