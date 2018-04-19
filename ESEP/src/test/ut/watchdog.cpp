#include <chrono>

#include "test/ut/watchdog.h"

#include "test/unit/assertions.h"

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/timer.h"

#include "serial/connection.h"
#include "serial/bsp_client.h"
#include "serial/dummy_connection.h"

#define MXT_TIMEOUT 10

namespace esep { namespace test { namespace unit {

namespace
{
	inline uint diff(uint a, uint b) { return a > b ? a - b : b - a; }
}

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

//	mWatchdog[0] = new serial::BSPClient(connection_ptr(mConnections[0]));
//	mWatchdog[1] = new serial::BSPClient(connection_ptr(mConnections[1]));

	client_ptr c0(new serial::BSPClient(connection_ptr(mConnections[0])));
	client_ptr c1(new serial::BSPClient(connection_ptr(mConnections[1])));

	mWatchdog[0] = new serial::Watchdog(std::move(c0), MXT_TIMEOUT);
	mWatchdog[1] = new serial::Watchdog(std::move(c1), MXT_TIMEOUT);

	lib::Timer::Class::sleep(100);
	MXT_LOG("Done setting up");

}

void Watchdog::teardown(void)
{
	MXT_LOG("deleting watchdogs");
	delete mWatchdog[0];
	delete mWatchdog[1];
	MXT_LOG("done");
//	delete mConnections[0];
//	delete mConnections[1];
}

void Watchdog::define(void)
{
	UNIT_TEST("can setup a watchdog")
	{
//		auto r = lib::Timer::instance().elapsed();


//		MXT_LOG(lib::stringify("Has sent ", mConnections[0]->getSentPackets()));

//		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		lib::Timer::Class::sleep(100);

//		lib::Timer::Class::sleep(MXT_TIMEOUT / 2 * 10);

//		r = lib::Timer::instance().elapsed() - r;
//
//		ASSERT_TRUE(diff(mConnections[0]->getSentPackets(), r / (MXT_TIMEOUT / 2)) <= 1);
	};
}

}}}
