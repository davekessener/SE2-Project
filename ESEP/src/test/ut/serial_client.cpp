#include <chrono>

#include "test/ut/serial_client.h"

#include "test/unit/assertions.h"

#include "serial/bsp_client.h"

#include "lib/logger.h"

#define MXT_TIMEOUT 30

namespace esep { namespace test { namespace unit {

namespace
{
	class RNG
	{
		public:
			template<size_t N>
			RNG(const byte_t (&a)[N], uint c = 100)
				: mSource(&a[0]), mSize(N), mChance(c), memory(0) { }
			byte_t operator()(byte_t v, uint i)
			{
				memory += i;
				memory = (memory << 2) ^ v ^ mSource[memory % mSize];

				if(!(memory % mChance))
				{
					v = memory;
				}

				return v;
			}
		private:
			const byte_t * const mSource;
			const size_t mSize;
			const uint mChance;
			uint32_t memory;
	};
}



SerialClient::SerialClient(void)
	: TestSuite("Serial Client (BSP)")
{
}

void SerialClient::setup(void)
{
	typedef std::unique_ptr<serial::Connection> connection_ptr;

	mConnections[0] = new serial::DummyConnection;
	mConnections[1] = new serial::DummyConnection;

	mConnections[0]->connect(*mConnections[1]);

	mClients[0] = new serial::BSPClient(connection_ptr(mConnections[0]), MXT_TIMEOUT);
	mClients[1] = new serial::BSPClient(connection_ptr(mConnections[1]), MXT_TIMEOUT);
}

void SerialClient::teardown(void)
{
	delete mClients[0]; mClients[0] = nullptr;
	delete mClients[1]; mClients[1] = nullptr;
//	delete mConnections[0]; Owned by clients
//	delete mConnections[1]; Owned by clients
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

		lib::Timer::instance().sleep(5);

		auto r1 = mClients[1]->read();

		lib::Timer::instance().sleep(5);

		mClients[1]->write(r1);

		lib::Timer::instance().sleep(5);

		auto r2 = mClients[0]->read();

		lib::Timer::instance().sleep(5);

		ASSERT_EACH_EQUALS(r2, cmp);
	};

	UNIT_TEST("does not retransmit packets unecessarily")
	{
		std::string cmp("Hello, World!");
		serial::Client::buffer_t buf(cmp.cbegin(), cmp.cend());

		mClients[0]->write(buf);

		lib::Timer::instance().sleep(MXT_TIMEOUT * 3);

		ASSERT_EQUALS(mConnections[0]->getSentPackets(), 1u);
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

	UNIT_TEST("does request retransmission on a simple error")
	{
		bool running = true;

		mConnections[0]->setTransform([&running](byte_t v, uint i) {
			if(running && i == 10)
			{
				v = ~v;
				running = false;
			}

			return v;
		});

		std::string cmp("Hello, world!");

		mClients[0]->write(serial::Client::buffer_t(cmp.cbegin(), cmp.cend()));

		auto r = mClients[1]->read();

		ASSERT_EACH_EQUALS(r, cmp);
	};

	UNIT_TEST("can handle a reset")
	{
		bool running = true;

		mConnections[0]->setTransform([&running](byte_t v, uint i) {
			if(running && i == 0)
			{
				v = 0xDA;
				running = false;
			}

			return v;
		});

		std::string cmp("Hello, World!");

		mClients[0]->write(serial::Client::buffer_t(cmp.cbegin(), cmp.cend()));

		auto r = mClients[1]->read();

		ASSERT_EACH_EQUALS(r, cmp);
	};

	UNIT_TEST("can handle an unreliable connection")
	{
		static const byte_t rand[] = { 17, 237, 3, 71, 93, 181, 182, 115, 76, 170 };
		RNG unreliable[2] = {{rand, 500}, {rand, 500}};

		std::vector<byte_t> cmp;

		for(uint i = 0 ; i < 10000 ; ++i)
		{
			cmp.push_back(i * i);
		}

		mConnections[0]->setTransform(unreliable[0]);
		mConnections[1]->setTransform(unreliable[1]);

		mClients[0]->write(cmp);

		auto r = mClients[1]->read();

		ASSERT_EACH_EQUALS(r, cmp);
	};

	UNIT_TEST("keeps sending packets if no confimation")
	{
		mConnections[0]->kill();

		ASSERT_EQUALS(mConnections[0]->getSentPackets(), 0u);

		serial::Client::buffer_t b;
		b.push_back(42);

		mClients[0]->write(b);

		lib::Timer::instance().sleep(MXT_TIMEOUT * 10 + MXT_TIMEOUT / 2);

		ASSERT_APPROX_EQUALS(mConnections[0]->getSentPackets(), 10u);
	};
}

}}}
