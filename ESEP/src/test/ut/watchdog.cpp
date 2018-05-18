#include <chrono>

#include "test/ut/watchdog.h"

#include "test/unit/assertions.h"

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/timer.h"

#include "serial/connection.h"
#include "serial/bsp_client.h"

#define MXT_TIMEOUT 50
#define MXT_MAXTRIES 100

namespace esep { namespace test { namespace unit {

Watchdog::Watchdog(void)
	: TestSuite("Watchdog")
{
	mWatchdog[0] = mWatchdog[1] = nullptr;
	mConnections[0] = mConnections[1] = nullptr;
}

void Watchdog::setup(void)
{
	typedef std::unique_ptr<serial::Connection> connection_ptr;
	typedef std::unique_ptr<serial::Client> client_ptr;

	mConnections[0] = new serial::DummyConnection;
	mConnections[1] = new serial::DummyConnection;

	mConnections[0]->connect(*mConnections[1]);

	client_ptr c0(new serial::BSPClient(connection_ptr(mConnections[0])));
	client_ptr c1(new serial::BSPClient(connection_ptr(mConnections[1])));

	mWatchdog[0] = new serial::Watchdog(std::move(c0), MXT_TIMEOUT);
	mWatchdog[1] = new serial::Watchdog(std::move(c1), MXT_TIMEOUT);

	for(uint i = MXT_MAXTRIES ; !mWatchdog[0]->connected() || !mWatchdog[1]->connected() ; --i)
	{
		if(!i)
		{
			MXT_THROW_EX(InstantiationException);
		}

		lib::Timer::instance().sleep(10);
	}
}

void Watchdog::teardown(void)
{
	delete mWatchdog[0];
	delete mWatchdog[1];
//	delete mConnections[0];
//	delete mConnections[1];
}

void Watchdog::define(void)
{
	UNIT_TEST("can setup a watchdog")
	{
	};

	UNIT_TEST("sends packets")
	{
		lib::Timer::instance().sleep(10 * (MXT_TIMEOUT / 2));

		ASSERT_APPROX_EQUALS(mConnections[0]->getSentPackets() / 2, 8u);
	};

	UNIT_TEST("can communicate")
	{
		std::string cmp("Hello, World!");
		serial::Client::buffer_t buf(cmp.cbegin(), cmp.cend());

		mWatchdog[0]->write(buf);

		lib::Timer::instance().sleep(5);

		auto r1 = mWatchdog[1]->read();

		lib::Timer::instance().sleep(5);

		mWatchdog[1]->write(r1);

		lib::Timer::instance().sleep(5);

		auto r2 = mWatchdog[0]->read();

		lib::Timer::instance().sleep(5);

		ASSERT_EACH_EQUALS(r2, cmp);
	};

	UNIT_TEST("can detect a dead line")
	{
		std::string cmp("Hello, World!");
		serial::Client::buffer_t buf(cmp.cbegin(), cmp.cend());

		mWatchdog[0]->write(buf);

		lib::Timer::instance().sleep(5);

		ASSERT_EACH_EQUALS(mWatchdog[1]->read(), buf);

		lib::Timer::instance().sleep(5);

		mConnections[0]->kill();
		mConnections[1]->kill();

		lib::Timer::instance().sleep(MXT_TIMEOUT * 2);

		ASSERT_FAILURE(mWatchdog[0]->read(), serial::Connection::ConnectionClosedException);
		ASSERT_FAILURE(mWatchdog[1]->read(), serial::Connection::ConnectionClosedException);
	};
}

}}}
