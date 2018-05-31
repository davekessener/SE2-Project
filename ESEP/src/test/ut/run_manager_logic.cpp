#include "test/ut/run_manager_logic.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace ut {


typedef communication::Packet_ptr Packet_ptr;

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
	mRunManager = new base::RunManager(mCom, mConfig);
}

void RunManagerLogic::teardown(void)
{
		delete mConfig; mConfig = nullptr;
		delete mCom; mCom = nullptr;
		delete mRunManager; mRunManager = nullptr;
}

void RunManagerLogic::define(void)
{

}

}}}
