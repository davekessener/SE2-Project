
#include "test/ut/error_manager_logic.h"

#include "test/unit/assertions.h"

#include "hal.h"

namespace esep { namespace test { namespace ut {

typedef communication::Packet Packet;
typedef communication::Packet_ptr Packet_ptr;
typedef data::Data_ptr Data_ptr;

struct BasicRecipient : public communication::IRecipient
{
	void accept(Packet_ptr p) override
	{
		packets.push_back(p);
	}

	std::vector<Packet_ptr> packets;
};

ErrorManagerLogic::ErrorManagerLogic(void)
	: TestSuite("Error Manager Logic")
	, mErrManager(nullptr)
	, mHandler(nullptr)
{

}

void ErrorManagerLogic::setup(void)
{
	mHandler = new BasicRecipient;
	mErrManager = new base::ErrorManager(mHandler);

	hal().setCallback( [this](Event e) { mErrManager->handle(e); } );
}

void ErrorManagerLogic::teardown(void)
{
	delete mErrManager;
	delete mHandler;
}

void ErrorManagerLogic::send(Location to, msg_t msg)
{
	mErrManager->accept(std::make_shared<Packet>(Location::MASTER, to, msg));
}

void ErrorManagerLogic::sendl(Location to, msg_t msg, dloc_t loc)
{
	Packet_ptr packet = std::make_shared<Packet>(Location::MASTER, to, msg);
	packet->addDataPoint(Data_ptr(new data::Location(loc)));
	mErrManager->accept(packet);
}

void ErrorManagerLogic::define()
{
	UNIT_TEST("can acknowledge estop error")
	{
		mErrManager->enter();

		hal().setField(Field::GPIO_0, !static_cast<uint32_t>(Button::ESTOP));
		send(Location::BASE, Message::Error::ESTOP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::ESTOP));
		hal().trigger(Event::BTN_ESTOP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		ASSERT_EQUALS(mHandler->packets.front()->source(), Location::BASE);
		ASSERT_EQUALS(mHandler->packets.front()->target(), Location::MASTER);
		ASSERT_EQUALS(mHandler->packets.front()->message(), Message::Master::FIXED);
	};

	UNIT_TEST("won't leave estop error with wrong acknowledgment")
	{
		mErrManager->enter();

		hal().setField(Field::GPIO_0, !static_cast<uint32_t>(Button::ESTOP));
		send(Location::BASE, Message::Error::ESTOP);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::ESTOP));
		hal().trigger(Event::BTN_ESTOP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);
	};

	UNIT_TEST("can acknowledge ramp error")
	{
		mErrManager->enter();
		send(Location::BASE, Message::Error::RAMP_FULL);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_RAMP));	// this shouldnt work
		hal().trigger(Event::LB_RAMP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		ASSERT_EQUALS(mHandler->packets.front()->source(), Location::BASE);
		ASSERT_EQUALS(mHandler->packets.front()->target(), Location::MASTER);
		ASSERT_EQUALS(mHandler->packets.front()->message(), Message::Master::FIXED);
	};

	UNIT_TEST("can set warning error")
	{
		mErrManager->enter();
		send(Location::BASE, Message::Error::WARNING);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		ASSERT_EQUALS(mHandler->packets.front()->source(), Location::BASE);
		ASSERT_EQUALS(mHandler->packets.front()->target(), Location::MASTER);
		ASSERT_EQUALS(mHandler->packets.front()->message(), Message::Master::FIXED);
	};

	UNIT_TEST("can acknowledge item appeared error")
	{
		// LB_HEIGHTSENSOR TEST
		mErrManager->enter();
		sendl(Location::BASE, Message::Error::ITEM_APPEARED, dloc_t::LB_HEIGHTSENSOR);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_HEIGHTSENSOR));
		hal().trigger(Event::LB_HEIGHTSENSOR);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		ASSERT_EQUALS(mHandler->packets.front()->source(), Location::BASE);
		ASSERT_EQUALS(mHandler->packets.front()->target(), Location::MASTER);
		ASSERT_EQUALS(mHandler->packets.front()->message(), Message::Master::FIXED);

		delete mErrManager;
		mErrManager = new base::ErrorManager(mHandler);

		// LB_START TEST
		mErrManager->enter();
		sendl(Location::BASE, Message::Error::ITEM_APPEARED, dloc_t::LB_START);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_START));
		hal().trigger(Event::LB_START);
		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 2u);

		delete mErrManager;
		mErrManager = new base::ErrorManager(mHandler);

		// LB_SWITCH TEST
		mErrManager->enter();
		sendl(Location::BASE, Message::Error::ITEM_APPEARED, dloc_t::LB_SWITCH);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_SWITCH));
		hal().trigger(Event::LB_SWITCH);
		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 3u);

		delete mErrManager;
		mErrManager = new base::ErrorManager(mHandler);

		// LB_RAMP TEST
		mErrManager->enter();
		sendl(Location::BASE, Message::Error::ITEM_APPEARED, dloc_t::LB_RAMP);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_RAMP));
		hal().trigger(Event::LB_RAMP);
		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 4u);

		delete mErrManager;
		mErrManager = new base::ErrorManager(mHandler);

		// LB_END TEST
		mErrManager->enter();
		sendl(Location::BASE, Message::Error::ITEM_APPEARED, dloc_t::LB_END);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_END));
		hal().trigger(Event::LB_END);
		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 5u);
	};

	UNIT_TEST("item appeared can be fixed after acknowledgment")
	{
		mErrManager->enter();
		sendl(Location::BASE, Message::Error::ITEM_APPEARED, dloc_t::LB_END);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_END));
		hal().trigger(Event::LB_END);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);
	};

	UNIT_TEST("can acknowledge item disappeared error")
	{
		mErrManager->enter();
		send(Location::BASE, Message::Error::ITEM_DISAPPEARED);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		ASSERT_EQUALS(mHandler->packets.front()->source(), Location::BASE);
		ASSERT_EQUALS(mHandler->packets.front()->target(), Location::MASTER);
		ASSERT_EQUALS(mHandler->packets.front()->message(), Message::Master::FIXED);
	};

	UNIT_TEST("can switch to higher prioritized error")
	{
		mErrManager->enter();
		send(Location::BASE, Message::Error::WARNING);
		hal().setCallback( [this](Event e) { mErrManager->handle(e); } );

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		send(Location::BASE, Message::Error::RAMP_FULL);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(LightBarrier::LB_RAMP));	// this shouldnt work
		hal().trigger(Event::LB_RAMP);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 2u);
	};

	UNIT_TEST("switching to lower prioritized error will not work")
	{
		mErrManager->enter();

		hal().setField(Field::GPIO_0, !static_cast<uint32_t>(Button::ESTOP));
		send(Location::BASE, Message::Error::ESTOP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		send(Location::BASE, Message::Error::WARNING);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::ESTOP));
		hal().trigger(Event::BTN_ESTOP);


		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		send(Location::BASE, Message::Error::WARNING);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		mErrManager->leave();
		mErrManager->enter();

		send(Location::BASE, Message::Error::WARNING);

		ASSERT_EQUALS(mHandler->packets.size(), 2u);
	};

//	UNIT_TEST("can set (irrecoverable) serial error")
//	{
//		mErrManager->enter();
//		send(Location::BASE, Message::Error::SERIAL);
//		ASSERT_EQUALS(mHandler->packets.size(), 0u);
//	};
//
//	UNIT_TEST("can set (irrecoverable) config error")
//	{
//		mErrManager->enter();
//		send(Location::BASE, Message::Error::SERIAL);
//		ASSERT_EQUALS(mHandler->packets.size(), 0u);
//	};
}

}}}
