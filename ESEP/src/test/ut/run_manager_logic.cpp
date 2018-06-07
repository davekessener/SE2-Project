#include <iostream>
#include "test/ut/run_manager_logic.h"

#include "test/unit/assertions.h"
#include "test/unit/hal.h"
#include "lib/logger.h"
#include "lib/enum.h"
#include "communication/message.h"

namespace esep
{
namespace test
{
namespace ut
{

#define MXT_SLEEP(t)		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(t)))
#define MXT_BM_SWITCH 		(1u << 19)
#define MXT_BM_MOTOR_START	(1u << 12)

struct RunManagerLogic::HandlerDummy: public communication::IRecipient
{
	typedef communication::Packet_ptr Packet_ptr;

	HandlerDummy() : mBase(nullptr) { }

	void accept(Packet_ptr p) override
	{
		if (p->target() == communication::Packet::Location::BASE)
		{
			for (data::Data_ptr d : *p)
			{
				if(d->type() == data::DataPoint::Type::RUN_MANAGER_TIMER)
				{
					data::RunManagerTimer * data = dynamic_cast<data::RunManagerTimer *>(d.get());
					MXT_LOG_WARN("DUMMY: redirect timer event: ", data->event());
				}
			}

			(mBase != nullptr) ? mBase->accept(p) : MXT_LOG_WARN("HandlerDummy needs a valid pointer to base!");
		}
		else
		{
			MXT_LOG_WARN("DUMMY: got packet: ", p," MSG: ", p->message().as<Message::Run>());
			mPackets.push_back(p);
		}
	}
	std::deque<Packet_ptr> mPackets;
	communication::IRecipient * mBase;
};

RunManagerLogic::RunManagerLogic()
	: TestSuite("Run Manager")
{
	mRunManager = nullptr;
	mHandlerDummy = nullptr;
	mConfig = nullptr;
}

void RunManagerLogic::setup(void)
{
	mConfig = new config_t("ut.conf");
	mConfig->setStartToHs(1000);
	mConfig->setHsToSwitch(1000);
	mConfig->setSwitchToEnd(1000);
	mConfig->setMaxHandOverTime(500);
	mConfig->setHeightSensorMax(10);
	mConfig->setHeightSensorMin(10);
	mConfig->setSlowFactor(1);
	mConfig->setTimeTolerance(0.2);

	mHandlerDummy = new HandlerDummy;
	mRunManager = new base::RunManager(mHandlerDummy, mConfig);
	mHandlerDummy->mBase = this->mRunManager;

	hal().setCallback([this](Event e)
	{	mRunManager->handle(e);});
}

void RunManagerLogic::teardown(void)
{
	hal().setCallback([](Event){});

	delete mConfig;
	mConfig = nullptr;

	delete mHandlerDummy;
	mHandlerDummy = nullptr;

	delete mRunManager;
	mRunManager = nullptr;
}

void RunManagerLogic::sendPacket(msg_t msg)
{
	mRunManager->accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, msg));
}

uint32_t RunManagerLogic::maxTime(uint32_t t)
{
	return t * (1 + mConfig->TOLERANCE);
}

void RunManagerLogic::blockLB(LightBarrier lb)
{
	hal().setField(Field::GPIO_0,
			hal().getField(Field::GPIO_0) & ~static_cast<uint32_t>(lb));
}

void RunManagerLogic::freeLB(LightBarrier lb)
{
	hal().setField(Field::GPIO_0,
			hal().getField(Field::GPIO_0) | static_cast<uint32_t>(lb));
}

void RunManagerLogic::define(void)
{
	UNIT_TEST("Can Create RunManager")
	{

	};

	UNIT_TEST("resume and suspend")
	{
		sendPacket(Message::Run::RESUME);
		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_TRUE(hal().writes().back().get<uint32_t>() & MXT_BM_MOTOR_START);

		sendPacket(Message::Run::SUSPEND);
		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_FALSE(hal().writes().back().get<uint32_t>() & MXT_BM_MOTOR_START); //right comparrison?
	};

	UNIT_TEST("positive test - from start to end")
	{
		//Expect new -> LB_HS
		mRunManager->enter();
		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::NEW_ITEM);
		mHandlerDummy->mPackets.pop_front();

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		//LB_HS -> LB_SWITCH
		hal().trigger(Event::HEIGHT_SENSOR);
		hal().trigger(Event::HEIGHT_SENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::ANALYSE);
		mHandlerDummy->mPackets.pop_front();

		sendPacket(Message::Run::KEEP_NEXT);

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_SWITCH, MXT_BM_SWITCH);

		//LB_SWITCH -> LB_END

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(mConfig->switchToEnd());

		blockLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::REACHED_END);
		mHandlerDummy->mPackets.pop_front();

		freeLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::END_FREE);
		mHandlerDummy->mPackets.pop_front();
	};

	UNIT_TEST("positive test - from start to ramp")
	{
		mRunManager->enter();
		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::NEW_ITEM);
		mHandlerDummy->mPackets.pop_front();

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		hal().trigger(Event::HEIGHT_SENSOR);
		hal().trigger(Event::HEIGHT_SENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::ANALYSE);
		mHandlerDummy->mPackets.pop_front();

		//til now same as above

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		blockLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		freeLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::ITEM_REMOVED);
		mHandlerDummy->mPackets.pop_front();
	};

	UNIT_TEST("ramp full")
	{
		mRunManager->enter();
		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::NEW_ITEM);
		mHandlerDummy->mPackets.pop_front();

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		hal().trigger(Event::HEIGHT_SENSOR);
		hal().trigger(Event::HEIGHT_SENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::ANALYSE);
		mHandlerDummy->mPackets.pop_front();

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		blockLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		//til now same as above

		//TODO: i need a new conf time, for the ramp to trigger a full ramp
		MXT_SLEEP(mConfig->maxHandOverTime() * (1 + mConfig->TOLERANCE));

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.back()->message(), Message::Run::RAMP_FULL);
		mHandlerDummy->mPackets.pop_front();
	};

	UNIT_TEST("Item disappeared by handing over")
	{
		mRunManager->enter();
		sendPacket(Message::Run::EXPECT_NEW);
		ASSERT_TRUE(mHandlerDummy->mPackets.empty());

		MXT_SLEEP(mConfig->maxHandOverTime() * (1 + mConfig->TOLERANCE));
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
		ASSERT_EQUALS(mHandlerDummy->mPackets.front()->message(), Message::Run::ITEM_DISAPPEARED);

		MXT_SLEEP(mConfig->maxHandOverTime() * (1 + mConfig->TOLERANCE));
		ASSERT_EQUALS(mHandlerDummy->mPackets.size(), 1u);
	};

}

}
}
}
