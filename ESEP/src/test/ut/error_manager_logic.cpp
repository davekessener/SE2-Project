
#include "test/ut/error_manager_logic.h"

#include "test/unit/assertions.h"

#include "hal.h"

namespace esep { namespace test { namespace ut {

struct BasicRecipient : public communication::IRecipient
{
	typedef communication::Packet Packet;
	typedef communication::Packet_ptr Packet_ptr;

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
	delete mHandler;
	delete mErrManager;
}

void ErrorManagerLogic::send(Location to, msg_t msg)
{
	mErrManager->accept(std::make_shared<Packet>(Location::MASTER, to, msg));
}

void ErrorManagerLogic::define()
{
	UNIT_TEST("can acknowledge estop error")
	{
		mErrManager->enter();
		send(Location::BASE, Message::Error::ESTOP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::ESTOP));			// this shouldnt work
		hal().trigger(Event::BTN_ESTOP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::RESET));
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mHandler->packets.size(), 1u);

		ASSERT_EQUALS(mHandler->packets.front()->source(), Location::BASE);
		ASSERT_EQUALS(mHandler->packets.front()->target(), Location::MASTER);
		ASSERT_EQUALS(mHandler->packets.front()->message(), Message::Master::FIXED);
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

	UNIT_TEST("can switch to higher prioritized error")
	{
		mErrManager->enter();
		send(Location::BASE, Message::Error::WARNING);

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
		send(Location::BASE, Message::Error::ESTOP);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		send(Location::BASE, Message::Error::WARNING);

		ASSERT_EQUALS(mHandler->packets.size(), 0u);

		hal().setField(Field::GPIO_0, static_cast<uint32_t>(Button::ESTOP));			// this shouldnt work
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
