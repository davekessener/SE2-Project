#include <thread>
#include <chrono>

#include "test/ut/serial_client.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

SerialClient::SerialClient(void)
	: TestSuite("Serial Client (BSP)")
{
}

void SerialClient::setup(void)
{
	mConnections[0] = new serial::DummyConnection;
	mConnections[1] = new serial::DummyConnection;

	mConnections[0]->connect(*mConnections[1]);

	mClients[0] = new serial::Client(*mConnections[0]);
	mClients[1] = new serial::Client(*mConnections[1]);
}

void SerialClient::teardown(void)
{
	delete mClients[0];
	delete mClients[1];
	delete mConnections[0];
	delete mConnections[1];
}

void SerialClient::define(void)
{
	UNIT_TEST("can create a client object")
	{
	};

	UNIT_TEST("can send some data")
	{
		std::string cmp("Hello, World!");
		serial::Client::buffer_t buf(cmp.cbegin(), cmp.cend());

		mClients[0]->write(buf);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		auto r1 = mClients[1]->read();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		mClients[1]->write(r1);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		auto r2 = mClients[0]->read();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		ASSERT_EACH_EQUALS(r2, cmp);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	};

	UNIT_TEST("can send large amounts of data")
	{
		std::vector<byte_t> cmp;

		for(uint i = 0, a = 1, b = 1 ; i < 100 ; ++i)
		{
			a += b;

			std::swap(a, b);

			cmp.push_back(static_cast<byte_t>(a));
		}

		mClients[0]->write(cmp);

		auto r = mClients[1]->read();

		ASSERT_EACH_EQUALS(r, cmp);
	};

	UNIT_TEST("can send extremely large amounts of data (chaining)")
	{
		std::vector<byte_t> cmp;

		for(uint i = 0 ; i < 10000 ; ++i)
		{
			cmp.push_back(i * i);
		}

		mClients[0]->write(cmp);

		auto r = mClients[1]->read();

		ASSERT_EACH_EQUALS(r, cmp);
	};
}

}}}