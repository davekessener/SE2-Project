#include "test/ut/communication_layer.h"

#include "test/unit/assertions.h"

#include "communication/master.h"
#include "communication/slave.h"

#include "serial/bsp_client.h"
#include "serial/watchdog.h"

#include "lib/logger.h"

#define MXT_TIMEOUT 50

namespace esep { namespace test { namespace unit {

CommunicationLayer::CommunicationLayer(void)
	: TestSuite("Communication Layer")
{
	mMaster = mBaseM = mBaseS = nullptr;
	mConnections[0] = mConnections[1] = nullptr;
	mMasterCom = mSlaveCom = nullptr;
}

void CommunicationLayer::setup(void)
{
	typedef std::unique_ptr<serial::Connection> Connection_ptr;
	typedef std::unique_ptr<serial::Client> Client_ptr;

	mMaster = new SimpleRecipient;
	mBaseM = new SimpleRecipient;
	mBaseS = new SimpleRecipient;

	mConnections[0] = new serial::DummyConnection;
	mConnections[1] = new serial::DummyConnection;

	mConnections[0]->connect(*mConnections[1]);

	Connection_ptr con0(mConnections[0]), con1(mConnections[1]);
	Client_ptr bsp0(new serial::BSPClient(std::move(con0))), bsp1(new serial::BSPClient(std::move(con1)));
	Client_ptr c0(new serial::Watchdog(std::move(bsp0), MXT_TIMEOUT)), c1(new serial::Watchdog(std::move(bsp1), MXT_TIMEOUT));

	mMasterCom = new communication::Master(mMaster, std::move(c0));
	mSlaveCom = new communication::Slave(std::move(c1));

	mMasterCom->setBase(mBaseM);
	mSlaveCom->setBase(mBaseS);
}

void CommunicationLayer::teardown(void)
{
	delete mSlaveCom;
	delete mMasterCom;
//	delete mConnections[0];
//	delete mConnections[1];
	delete mBaseS;
	delete mBaseM;
	delete mMaster;
}

void CommunicationLayer::define(void)
{
	UNIT_TEST("can instanstiate communication layer")
	{
	};

	UNIT_TEST("can route from master to base_m")
	{
		mMasterCom->accept(packet(Location::MASTER, Location::BASE_M, Message::ANALYSE));

		wait();

		lib::Timer::instance().sleep(10);

		ASSERT_EQUALS(mMaster->packets.size(), 0u);
		ASSERT_EQUALS(mBaseM->packets.size(),  1u);
		ASSERT_EQUALS(mBaseS->packets.size(),  0u);
		ASSERT_EQUALS(mBaseM->packets.front()->target(),  Location::BASE);
		ASSERT_EQUALS(mBaseM->packets.front()->source(),  Location::MASTER);
		ASSERT_EQUALS(mBaseM->packets.front()->message(), Message::ANALYSE);
	};

	UNIT_TEST("can route from base_m to master")
	{
		mMasterCom->accept(packet(Location::BASE, Location::MASTER, Message::RESUME));

		wait();

		lib::Timer::instance().sleep(10);

		ASSERT_EQUALS(mMaster->packets.size(), 1u);
		ASSERT_EQUALS(mBaseM->packets.size(),  0u);
		ASSERT_EQUALS(mBaseS->packets.size(),  0u);
		ASSERT_EQUALS(mMaster->packets.front()->target(),  Location::MASTER);
		ASSERT_EQUALS(mMaster->packets.front()->source(),  Location::BASE_M);
		ASSERT_EQUALS(mMaster->packets.front()->message(), Message::RESUME);
	};

	UNIT_TEST("can route from base_s to master")
	{
		mSlaveCom->accept(packet(Location::BASE, Location::MASTER, Message::IDLE));

		lib::Timer::instance().sleep(10);

		wait();

		lib::Timer::instance().sleep(10);

		ASSERT_EQUALS(mMaster->packets.size(), 1u);
		ASSERT_EQUALS(mBaseM->packets.size(),  0u);
		ASSERT_EQUALS(mBaseS->packets.size(),  0u);
		ASSERT_EQUALS(mMaster->packets.front()->target(),  Location::MASTER);
		ASSERT_EQUALS(mMaster->packets.front()->source(),  Location::BASE_S);
		ASSERT_EQUALS(mMaster->packets.front()->message(), Message::IDLE);
	};

	UNIT_TEST("can broadcast from master")
	{
		mMasterCom->accept(packet(Location::MASTER, Location::BASE, Message::KEEP_NEXT));

		wait();

		lib::Timer::instance().sleep(10);

		ASSERT_EQUALS(mMaster->packets.size(), 0u);
		ASSERT_EQUALS(mBaseM->packets.size(),  1u);
		ASSERT_EQUALS(mBaseS->packets.size(),  1u);

		ASSERT_EQUALS(mBaseM->packets.front()->target(),  Location::BASE);
		ASSERT_EQUALS(mBaseM->packets.front()->source(),  Location::MASTER);
		ASSERT_EQUALS(mBaseM->packets.front()->message(), Message::KEEP_NEXT);

		ASSERT_EQUALS(mBaseS->packets.front()->target(),  Location::BASE);
		ASSERT_EQUALS(mBaseS->packets.front()->source(),  Location::MASTER);
		ASSERT_EQUALS(mBaseS->packets.front()->message(), Message::KEEP_NEXT);
	};

	UNIT_TEST("detects a dead connection")
	{
		mMasterCom->accept(packet(Location::BASE, Location::MASTER, Message::RESUME));

		wait();

		lib::Timer::instance().sleep(10);

		mConnections[0]->kill();

		lib::Timer::instance().sleep(MXT_TIMEOUT * 2);

		ASSERT_EQUALS(mMaster->packets.size(), 2u);
		ASSERT_EQUALS(mBaseM->packets.size(), 1u);
		ASSERT_EQUALS(mBaseS->packets.size(), 1u);

		ASSERT_EQUALS(mMaster->packets[0]->target(),  Location::MASTER);
		ASSERT_EQUALS(mMaster->packets[0]->source(),  Location::BASE_M);
		ASSERT_EQUALS(mMaster->packets[0]->message(), Message::RESUME);

		ASSERT_EQUALS(mMaster->packets[1]->target(),  Location::MASTER);
		ASSERT_EQUALS(mMaster->packets[1]->source(),  Location::BASE_M);
		ASSERT_EQUALS(mMaster->packets[1]->message(), Message::ERROR_SERIAL);

		ASSERT_EQUALS(mBaseM->packets.front()->target(),  Location::BASE);
		ASSERT_EQUALS(mBaseM->packets.front()->source(),  Location::MASTER);
		ASSERT_EQUALS(mBaseM->packets.front()->message(), Message::ERROR_SERIAL);

		ASSERT_EQUALS(mBaseS->packets.front()->target(),  Location::BASE);
		ASSERT_EQUALS(mBaseS->packets.front()->source(),  Location::MASTER);
		ASSERT_EQUALS(mBaseS->packets.front()->message(), Message::ERROR_SERIAL);
	};
}

}}}
