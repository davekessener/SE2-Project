#include "test/ut/run_manager_logic.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

#define MXT_SLEEP(t)	std::this_thread::sleep_for(std::chrono::milliseconds(t))


struct BasicRecipient : public communication::IRecipient
{
	typedef communication::Packet_ptr Packet_ptr;

	void accept(Packet_ptr p) override
	{
		mPackets.push_back(p);
	}
	std::vector<Packet_ptr> mPackets;
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
}

void RunManagerLogic::setup(void)
{
	mCom = new BasicRecipient;
	mRunManager = new base::RunManager(mCom, mConfig);
}

void RunManagerLogic::sendPacket(msg_t msg)
{
	mRunManager->accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, msg));
}

void RunManagerLogic::teardown(void)
{
		delete mConfig; mConfig = nullptr;
		delete mCom; mCom = nullptr;
		delete mRunManager; mRunManager = nullptr;
}

void RunManagerLogic::define(void)
{
	UNIT_TEST("Can Create RunManager")
	{

	};

	UNIT_TEST("Item disappeared at LB_START")
	{
		mRunManager->enter();
		sendPacket(runMessage_t::EXPECT_NEW);
		ASSERT_TRUE(mCom->mPackets.empty());

		//MXT_SLEEP(mConfig->);
		ASSERT_EQUALS(mCom->mPackets.size(), 1u);
		ASSERT_EQUALS(mCom->mPackets.front(), runMessage_t::ITEM_DISAPPEARED);
	};

}

}}}
