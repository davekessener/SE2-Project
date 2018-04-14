#include "test/ut/watchdog.h"

#include "test/unit/assertions.h"

#define MXT_TIMEOUT 10

namespace esep { namespace test { namespace unit {

Watchdog::Watchdog(void)
	: TestSuite("Watchdog")
{
	mConnections[0] = mConnections[1] = nullptr;
	mWatchdog[0] = mWatchdog[1] = nullptr;
}

void Watchdog::setup(void)
{
	mConnections[0] = new serial::DummyConnection;
	mConnections[1] = new serial::DummyConnection;

	mConnections[0]->connect(*mConnections[1]);

	mWatchdog[0] = new wd_t(std::unique_ptr<serial::Client>(new serial::Client(*mConnections[0])), MXT_TIMEOUT);
	mWatchdog[1] = new wd_t(std::unique_ptr<serial::Client>(new serial::Client(*mConnections[1])), MXT_TIMEOUT);
}

void Watchdog::teardown(void)
{
	delete mWatchdog[0];
	delete mWatchdog[1];
	delete mConnections[0];
	delete mConnections[1];
}

void Watchdog::define(void)
{
	UNIT_TEST("can setup a watchdog")
	{
	};
}

}}}
