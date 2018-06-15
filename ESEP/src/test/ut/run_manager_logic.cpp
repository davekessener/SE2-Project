#include <iostream>
#include <mutex>

#include "test/ut/run_manager_logic.h"
#include "lib/sync/container.h"
#include "test/unit/assertions.h"
#include "test/unit/hal.h"
#include "lib/logger.h"
#include "lib/enum.h"
#include "communication/message.h"
#include "lib/timer.h"

namespace esep
{
namespace test
{
namespace ut
{

#define MXT_SLEEP(t)		lib::Timer::instance().sleep(t * 3 / 2)
#define MXT_BM_LIGHT_G 		(1u << 18)
#define MXT_BM_SWITCH 		(1u << 19)
#define MXT_BM_MOTOR_START	(1u << 12)
#define MXT_BM_BTN_STOP		(1u << 23)

struct RunManagerLogic::HandlerDummy: public communication::IRecipient
{
	//typedef communication::Packet_ptr Packet_ptr;

	void accept(Packet_ptr p) override
	{
		std::lock_guard<std::mutex> lock(mPacketMutex);
		mMasterMsgs.emplace_back(p);
	}

	Packet_ptr takeFirstPacket()
	{
		std::lock_guard<std::mutex> lock(mPacketMutex);

		Packet_ptr p = mMasterMsgs.front();
		mMasterMsgs.pop_front();
		return p;
	}

	uint queueSize()
	{
		std::lock_guard<std::mutex> lock(mPacketMutex);
		return mMasterMsgs.size();
	}

	std::mutex mPacketMutex;
	std::deque<Packet_ptr> mMasterMsgs;
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
	mConfig = new config_t("ut.conf", 5, 0.1, 5, 5);

	mConfig->setStartToHs(10);
	mConfig->setHsToSwitch(10);
	mConfig->setSwitchToEnd(10);
	mConfig->setItemInLB(5);
	mConfig->setHeightSensorMax(10);
	mConfig->setHeightSensorMin(10);
	mConfig->setTimeTolerance(0.2);


	mHandlerDummy = new HandlerDummy;
	mRunManager = new base::RunManager(mHandlerDummy, mConfig);

	hal().setCallback([this](Event e) { mRunManager->handle(e);});
}

void RunManagerLogic::teardown(void)
{
	hal().setCallback([](Event){});

	delete mRunManager;
	mRunManager = nullptr;

	delete mHandlerDummy;
	mHandlerDummy = nullptr;

	delete mConfig;
	mConfig = nullptr;
}

void RunManagerLogic::sendPacket(msg_t msg)
{
	mRunManager->accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, msg));
}

uint32_t RunManagerLogic::maxTime(uint32_t t)
{
	return t * (1 + 2  * mConfig->tolerance());
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

void RunManagerLogic::createHeightmap(void)
{
	hal().setField(Field::ANALOG, 1);
	hal().trigger(Event::HEIGHT_SENSOR);
	hal().setField(Field::ANALOG, 0);
	hal().trigger(Event::HEIGHT_SENSOR);
}

void RunManagerLogic::define(void)
{
	UNIT_TEST("Can Create RunManager")
	{

	};

	UNIT_TEST("resume and suspend")
	{
		mRunManager->enter();
		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_LIGHT_G, MXT_BM_LIGHT_G);

		sendPacket(Message::Run::RESUME);
		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_TRUE(hal().writes().back().get<uint32_t>() & MXT_BM_MOTOR_START);


		sendPacket(Message::Run::SUSPEND);
		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_FALSE(hal().writes().back().get<uint32_t>() & MXT_BM_MOTOR_START);

		mRunManager->leave();
		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_LIGHT_G, 0u);
	};

	UNIT_TEST("positive test - from start to end")
	{
		//Expect new -> LB_HS
		mRunManager->enter();

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_LIGHT_G, MXT_BM_LIGHT_G);

		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::NEW_ITEM);

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		//LB_HS -> LB_SWITCH
		createHeightmap();

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::IN_HEIGHTSENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ANALYSE);

		sendPacket(Message::Run::KEEP_NEXT);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 0u);

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_SWITCH, MXT_BM_SWITCH);

		//LB_SWITCH -> LB_END

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(mConfig->switchToEnd());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::REACHED_END);

		freeLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::END_FREE);
	};

	UNIT_TEST("positive test - from start to ramp")
	{
		mRunManager->enter();
		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::NEW_ITEM);

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		createHeightmap();

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::IN_HEIGHTSENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ANALYSE);

		//til now same as above

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		blockLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		freeLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_REMOVED);
	};

	UNIT_TEST("ramp full")
	{
		mRunManager->enter();
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::NEW_ITEM);

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		createHeightmap();

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::IN_HEIGHTSENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ANALYSE);

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		blockLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		//til now same as above

		MXT_SLEEP(mConfig->rampTime());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 2u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_REMOVED);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::RAMP_FULL);
	};

	UNIT_TEST("Item disappeared by handing over")
	{
		mRunManager->enter();
		sendPacket(Message::Run::EXPECT_NEW);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 0u);

		MXT_SLEEP(mConfig->maxHandOverTime() * (1 + mConfig->tolerance()));

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_DISAPPEARED);

		MXT_SLEEP(mConfig->maxHandOverTime() * (1 + mConfig->tolerance()));

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 0u);
	};

	UNIT_TEST("Item appearing")
	{
		mRunManager->enter();

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_APPEARED);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_APPEARED);

		freeLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		blockLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_APPEARED);

		freeLB(LightBarrier::LB_RAMP);
		hal().trigger(Event::LB_RAMP);

		blockLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_APPEARED);

		freeLB(LightBarrier::LB_END);
		hal().trigger(Event::LB_END);

	};

	UNIT_TEST("Item stuck in LB_START")
	{
		mRunManager->enter();
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::NEW_ITEM);

		MXT_SLEEP(maxTime(mConfig->itemInLB()));

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_STUCK);
	};

	UNIT_TEST("Item stuck in LB_HS")
	{
		//Expect new -> LB_HS
		mRunManager->enter();

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_LIGHT_G, MXT_BM_LIGHT_G);

		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::NEW_ITEM);

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::IN_HEIGHTSENSOR);

		MXT_SLEEP(maxTime(mConfig->itemInLB()));

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_STUCK);
	};

	UNIT_TEST("Item stuck in LB_SWITCH")
	{
		//Expect new -> LB_HS
		mRunManager->enter();

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_LIGHT_G, MXT_BM_LIGHT_G);

		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::NEW_ITEM);

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		//LB_HS -> LB_SWITCH
		createHeightmap();

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::IN_HEIGHTSENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ANALYSE);

		MXT_SLEEP(maxTime(mConfig->itemInLB()));

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_STUCK);
	};

	UNIT_TEST("Item stuck in LB_SWITCH after KEEP_NEXT")
	{
		//Expect new -> LB_HS
		mRunManager->enter();

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_LIGHT_G, MXT_BM_LIGHT_G);

		sendPacket(Message::Run::EXPECT_NEW);
		blockLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::NEW_ITEM);

		freeLB(LightBarrier::LB_START);
		hal().trigger(Event::LB_START);

		MXT_SLEEP(mConfig->startToHs());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		Packet_ptr packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		//LB_HS -> LB_SWITCH
		createHeightmap();

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::IN_HEIGHTSENSOR);

		freeLB(LightBarrier::LB_HEIGHTSENSOR);
		hal().trigger(Event::LB_HEIGHTSENSOR);

		MXT_SLEEP(mConfig->hsToSwitch());

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		blockLB(LightBarrier::LB_SWITCH);
		hal().trigger(Event::LB_SWITCH);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ANALYSE);

		sendPacket(Message::Run::KEEP_NEXT);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 0u);

		ASSERT_EQUALS(hal().writes().back().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().back().get<uint32_t>() & MXT_BM_SWITCH, MXT_BM_SWITCH);

		MXT_SLEEP(maxTime(mConfig->itemInLB()));

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		packet = mHandlerDummy->takeFirstPacket();
		ASSERT_EQUALS(packet->message(), Message::Run::TIMER);
		mRunManager->accept(packet);

		MXT_SLEEP(1);
		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::ITEM_STUCK);
	};

	UNIT_TEST("requests stop")
	{
		mRunManager->enter();

		hal().setField(Field::GPIO_0, hal().getField(Field::GPIO_0) & ~MXT_BM_BTN_STOP);
		hal().trigger(Event::BTN_STOP);
		hal().setField(Field::GPIO_0, hal().getField(Field::GPIO_0) | MXT_BM_BTN_STOP);
		hal().trigger(Event::BTN_STOP);

		ASSERT_EQUALS(mHandlerDummy->queueSize(), 1u);
		ASSERT_EQUALS(mHandlerDummy->takeFirstPacket()->message(), Message::Run::REQUEST_STOP);
	};
}

}
}
}
