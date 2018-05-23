#include "test/ut/master_logic.h"

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

MasterLogic::MasterLogic(void)
	: TestSuite("Master Logic")
{
	mMaster = nullptr;
	mCom = nullptr;
}

void MasterLogic::setup(void)
{
	mCom = new BasicRecipient;
	mMaster = new master::Master(mCom);
}

void MasterLogic::teardown(void)
{
	delete mMaster; mMaster = nullptr;
	delete mCom; mCom = nullptr;
}

void MasterLogic::define(void)
{
	typedef communication::Packet Packet;
//	typedef communication::Packet_ptr Packet_ptr;
	typedef communication::Message Message;
	typedef Packet::Location Location;

	UNIT_TEST("can instantiate and destroy")
	{
	};

	UNIT_TEST("can switch to config")
	{
		mMaster->accept(std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Master::CONFIG));

		ASSERT_EQUALS(mCom->packets.size(), 1u);
		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);
	};

	UNIT_TEST("can switch to run")
	{
		mMaster->accept(std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Master::RUN));

		ASSERT_EQUALS(mCom->packets.size(), 1u);
		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::RUN);
	};

	UNIT_TEST("can switch through slave")
	{
		mMaster->accept(std::make_shared<Packet>(Location::BASE_S, Location::MASTER, Message::Master::CONFIG));

		ASSERT_EQUALS(mCom->packets.size(), 1u);
		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);
	};

	UNIT_TEST("doesn't double switch")
	{
		mMaster->accept(std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Master::CONFIG));

		ASSERT_EQUALS(mCom->packets.size(), 1u);
		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);

		mMaster->accept(std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Master::RUN));

		ASSERT_EQUALS(mCom->packets.size(), 1u);
		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);
	};

	UNIT_TEST("")
	{
	};
}

}}}
