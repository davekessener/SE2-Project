
#include "test/ut/error_manager_logic.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

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
		send(Location::BASE, Message::Error::ESTOP);

		mErrManager->handle(Event::BTN_ESTOP);
		mErrManager->handle(Event::BTN_RESET);
		mErrManager->handle(Event::BTN_START);

		//... here hal manipulation
		ASSERT_EQUALS(mHandler->packets.front()->source(), Location::BASE);
		ASSERT_EQUALS(mHandler->packets.front()->target(), Location::MASTER);
		ASSERT_EQUALS(mHandler->packets.front()->message(), Message::Master::FIXED);
	};
}

}}}
