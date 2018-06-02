#include "test/ut/master_logic.h"

#include "test/unit/assertions.h"

#include "data/data_point.h"
#include "data/location_data.h"

#include "lib/logger.h"

namespace esep { namespace test { namespace ut {

typedef data::Data_ptr Data_ptr;
typedef data::Location::Type LocationType;
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

struct SimplePlugin : master::Plugin
{
	SimplePlugin(Action a, float c = 1.0) : Plugin(Type::DEFAULT), action(a), chance(c) { }
	float match(const data_t&) override { return chance; }
	Action decide(const history_t&) override { return action; }

	const Action action;
	const float chance;
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
	mMaster = new master::Master(mCom, [this](const Item& item) { mItems.push_back(item); });

	mMaster->add(Plugin_ptr(new SimplePlugin(Plugin::Action::KEEP, 0.1f)));
}

void MasterLogic::teardown(void)
{
	mItems.clear();
	delete mMaster; mMaster = nullptr;
	delete mCom; mCom = nullptr;
}

void MasterLogic::send(Location from, msg_t msg)
{
	mMaster->accept(std::make_shared<Packet>(from, Location::MASTER, msg));
}

void MasterLogic::define(void)
{
	UNIT_TEST("can instantiate and destroy")
	{
	};

	UNIT_TEST("can switch to config")
	{
		send(Location::BASE_M, Message::Master::CONFIG);

		ASSERT_EQUALS(mCom->packets.size(), 1u);

		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);
	};

	UNIT_TEST("can switch to run")
	{
		send(Location::BASE_M, Message::Master::RUN);

		ASSERT_EQUALS(mCom->packets.size(), 1u);

		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::RUN);
	};

	UNIT_TEST("can switch through slave")
	{
		send(Location::BASE_S, Message::Master::CONFIG);

		ASSERT_EQUALS(mCom->packets.size(), 1u);

		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);
	};

	UNIT_TEST("doesn't double switch")
	{
		send(Location::BASE_M, Message::Master::CONFIG);

		ASSERT_EQUALS(mCom->packets.size(), 1u);

		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);

		send(Location::BASE_M, Message::Master::RUN);

		ASSERT_EQUALS(mCom->packets.size(), 1u);

		ASSERT_EQUALS(mCom->packets.front()->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets.front()->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets.front()->message(), Message::Base::CONFIG);
	};

	UNIT_TEST("starts module 1 when new item is placed")
	{
		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);

		ASSERT_EQUALS(mCom->packets.size(), 2u);

		ASSERT_EQUALS(mCom->packets[1]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[1]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[1]->message(), Message::Run::RESUME);
	};

	UNIT_TEST("can route past switch")
	{
		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);

		ASSERT_EQUALS(mCom->packets.size(), 3u);

		ASSERT_EQUALS(mCom->packets[1]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[1]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[1]->message(), Message::Run::RESUME);

		ASSERT_EQUALS(mCom->packets[2]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[2]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[2]->message(), Message::Run::KEEP_NEXT);
	};

	UNIT_TEST("can route to module 2")
	{
		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		ASSERT_EQUALS(mCom->packets.size(), 5u);

		ASSERT_EQUALS(mCom->packets[3]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[3]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[3]->message(), Message::Run::RESUME);

		ASSERT_EQUALS(mCom->packets[4]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[4]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[4]->message(), Message::Run::EXPECT_NEW);
	};

	UNIT_TEST("turns off module 1 when empty")
	{
		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);

		ASSERT_EQUALS(mCom->packets.size(), 6u);

		ASSERT_EQUALS(mCom->packets[5]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[5]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[5]->message(), Message::Run::SUSPEND);

		ASSERT_EQUALS(mItems.size(), 0u);
	};

	UNIT_TEST("can route throgh entire system")
	{
		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);
		send(Location::BASE_S, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_S, Message::Run::ANALYSE);

		ASSERT_EQUALS(mCom->packets.size(), 7u);

		ASSERT_EQUALS(mCom->packets[6]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[6]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[6]->message(), Message::Run::KEEP_NEXT);

		send(Location::BASE_S, Message::Run::KEEP_NEXT);
		send(Location::BASE_S, Message::Run::REACHED_END);

		ASSERT_EQUALS(mCom->packets.size(), 8u);

		ASSERT_EQUALS(mCom->packets[7]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[7]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[7]->message(), Message::Run::SUSPEND);

		ASSERT_EQUALS(mItems.size(), 1u);
	};

	UNIT_TEST("stops module 1 while module 2 is running")
	{
		send(Location::BASE_M, Message::Master::RUN);

		send(Location::BASE_M, Message::Run::NEW_ITEM);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::IN_HEIGHTSENSOR);

		ASSERT_EQUALS(mCom->packets.size(), 9u);

		ASSERT_EQUALS(mCom->packets[8]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[8]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[8]->message(), Message::Run::SUSPEND);
	};

	UNIT_TEST("doesn't want to keep an item")
	{
		mMaster->add(Plugin_ptr(new SimplePlugin(Plugin::Action::TOSS, 1.0f)));

		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);

		ASSERT_EQUALS(mCom->packets.size(), 2u);

		ASSERT_EQUALS(mCom->packets[1]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[1]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[1]->message(), Message::Run::RESUME);
	};

	UNIT_TEST("turns off after discarding an item")
	{
		mMaster->add(Plugin_ptr(new SimplePlugin(Plugin::Action::TOSS, 1.0f)));

		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::ITEM_REMOVED);

		ASSERT_EQUALS(mCom->packets.size(), 3u);

		ASSERT_EQUALS(mCom->packets[2]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[2]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[2]->message(), Message::Run::SUSPEND);
	};

	UNIT_TEST("can discard on slave")
	{
		mMaster->add(Plugin_ptr(new SimplePlugin(Plugin::Action::TOSS_S, 1.0f)));

		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);

		ASSERT_EQUALS(mCom->packets.size(), 3u);

		ASSERT_EQUALS(mCom->packets[2]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[2]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[2]->message(), Message::Run::KEEP_NEXT);

		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);
		send(Location::BASE_S, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_S, Message::Run::ANALYSE);

		ASSERT_EQUALS(mCom->packets.size(), 6u);

		ASSERT_EQUALS(mCom->packets[3]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[3]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[3]->message(), Message::Run::RESUME);

		ASSERT_EQUALS(mCom->packets[4]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[4]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[4]->message(), Message::Run::EXPECT_NEW);

		ASSERT_EQUALS(mCom->packets[5]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[5]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[5]->message(), Message::Run::SUSPEND);

		send(Location::BASE_S, Message::Run::ITEM_REMOVED);

		ASSERT_EQUALS(mCom->packets.size(), 7u);

		ASSERT_EQUALS(mCom->packets[6]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[6]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[6]->message(), Message::Run::SUSPEND);
	};

	UNIT_TEST("reroutes when ramp full")
	{
		mMaster->add(Plugin_ptr(new SimplePlugin(Plugin::Action::TOSS, 1.0f)));

		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::RAMP_FULL);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);

		ASSERT_EQUALS(mCom->packets.size(), 3u);

		ASSERT_EQUALS(mCom->packets[2]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[2]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[2]->message(), Message::Run::KEEP_NEXT);

		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);
		send(Location::BASE_S, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_S, Message::Run::ANALYSE);

		ASSERT_EQUALS(mCom->packets.size(), 6u);

		ASSERT_EQUALS(mCom->packets[3]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[3]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[3]->message(), Message::Run::RESUME);

		ASSERT_EQUALS(mCom->packets[4]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[4]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[4]->message(), Message::Run::EXPECT_NEW);

		ASSERT_EQUALS(mCom->packets[5]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[5]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[5]->message(), Message::Run::SUSPEND);
	};

	UNIT_TEST("both ramps full results in an error")
	{
		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::RAMP_FULL);
		send(Location::BASE_S, Message::Run::RAMP_FULL);

		ASSERT_EQUALS(mCom->packets.size(), 2u);

		ASSERT_EQUALS(mCom->packets[1]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[1]->target(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[1]->message(), Message::Error::RAMP_FULL);

		send(Location::MASTER, Message::Error::RAMP_FULL);

		ASSERT_EQUALS(mCom->packets.size(), 3u);

		ASSERT_EQUALS(mCom->packets[2]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[2]->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets[2]->message(), Message::Error::RAMP_FULL);
	};

	UNIT_TEST("can handle a dissappeared item")
	{
		send(Location::BASE_M, Message::Master::RUN);
		send(Location::BASE_M, Message::Run::NEW_ITEM);

		auto p = std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Run::ITEM_DISAPPEARED);

		p->addDataPoint(Data_ptr(new data::Location(LocationType::LB_HEIGHTSENSOR)));

		mMaster->accept(p);

		ASSERT_EQUALS(mCom->packets.size(), 4u);

		ASSERT_EQUALS(mCom->packets[2]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[2]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[2]->message(), Message::Run::SUSPEND);

		ASSERT_EQUALS(mCom->packets[3]->source(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[3]->target(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[3]->message(), Message::Error::ITEM_DISAPPEARED);

		mMaster->accept(Packet_ptr(new Packet(*mCom->packets.back())));

		ASSERT_EQUALS(mCom->packets.size(), 6u);

		ASSERT_EQUALS(mCom->packets[4]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[4]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[4]->message(), Message::Error::WARNING);

		ASSERT_EQUALS(mCom->packets[5]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[5]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[5]->message(), Message::Error::ITEM_DISAPPEARED);
	};

	UNIT_TEST("can recover from error")
	{
		send(Location::BASE_M, Message::Master::RUN);

		send(Location::BASE_M, Message::Run::NEW_ITEM);

		auto p = std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Run::ITEM_DISAPPEARED);

		p->addDataPoint(Data_ptr(new data::Location(LocationType::LB_HEIGHTSENSOR)));

		mMaster->accept(p);
		mMaster->accept(Packet_ptr(new Packet(*mCom->packets.back())));

		ASSERT_EQUALS(mCom->packets.size(), 6u);

		send(Location::BASE_M, Message::Master::FIXED);

		ASSERT_EQUALS(mCom->packets.size(), 6u);

		send(Location::BASE_S, Message::Master::FIXED);

		ASSERT_EQUALS(mCom->packets.size(), 7u);

		ASSERT_EQUALS(mCom->packets[6]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[6]->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets[6]->message(), Message::Base::READY);

		send(Location::BASE_M, Message::Master::RUN);

		ASSERT_EQUALS(mCom->packets.size(), 8u);

		ASSERT_EQUALS(mCom->packets[7]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[7]->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets[7]->message(), Message::Base::RUN);
	};

	UNIT_TEST("can recover from error under load")
	{
		send(Location::BASE_M, Message::Master::RUN);

		send(Location::BASE_M, Message::Run::NEW_ITEM);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);

		send(Location::BASE_M, Message::Run::NEW_ITEM);

		auto p = std::make_shared<Packet>(Location::BASE_M, Location::MASTER, Message::Run::ITEM_DISAPPEARED);

		p->addDataPoint(Data_ptr(new data::Location(LocationType::LB_HEIGHTSENSOR)));

		mMaster->accept(p);
		mMaster->accept(Packet_ptr(new Packet(*mCom->packets.back())));

		send(Location::BASE_S, Message::Master::FIXED);
		send(Location::BASE_M, Message::Master::FIXED);

		ASSERT_EQUALS(mCom->packets.size(), 11u);

		ASSERT_EQUALS(mCom->packets[10]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[10]->target(), Location::BASE);
		ASSERT_EQUALS(mCom->packets[10]->message(), Message::Base::READY);
	};

	UNIT_TEST("doesn't allow for more than one")
	{
		send(Location::BASE_M, Message::Master::RUN);

		send(Location::BASE_M, Message::Run::NEW_ITEM);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);

		send(Location::BASE_M, Message::Run::NEW_ITEM);
		send(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR);
		send(Location::BASE_M, Message::Run::ANALYSE);
		send(Location::BASE_M, Message::Run::KEEP_NEXT);
		send(Location::BASE_M, Message::Run::REACHED_END);
		send(Location::BASE_M, Message::Run::END_FREE);

		send(Location::BASE_S, Message::Run::NEW_ITEM);

		ASSERT_EQUALS(mCom->packets.size(), 10u);

		ASSERT_EQUALS(mCom->packets[9]->source(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[9]->target(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[9]->message(), Message::Error::ITEM_APPEARED);

		mMaster->accept(Packet_ptr(new Packet(*mCom->packets.back())));

		ASSERT_EQUALS(mCom->packets.size(), 12u);

		ASSERT_EQUALS(mCom->packets[10]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[10]->target(), Location::BASE_M);
		ASSERT_EQUALS(mCom->packets[10]->message(), Message::Error::WARNING);

		ASSERT_EQUALS(mCom->packets[11]->source(), Location::MASTER);
		ASSERT_EQUALS(mCom->packets[11]->target(), Location::BASE_S);
		ASSERT_EQUALS(mCom->packets[11]->message(), Message::Error::ITEM_APPEARED);
	};
}

}}}
