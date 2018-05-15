#include <chrono>
#include <vector>

#include "test/ut/dummy_connection.h"
#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

namespace
{
	void send(serial::Connection *c1, serial::Connection *c2)
	{
		std::string cmp("Hello, World!");

		c1->write(reinterpret_cast<const byte_t *>(cmp.c_str()), cmp.size());

		std::vector<byte_t> r(cmp.size());

		ASSERT_EQUALS(r.size(), cmp.size());

		c2->read(&r[0], r.size());

		ASSERT_EACH_EQUALS(r, cmp);
	}
}

DummyConnection::DummyConnection(void)
	: TestSuite("Serial Connection (Dummy)")
{
}

void DummyConnection::setup(void)
{
	mConnections[0] = new serial::DummyConnection;
	mConnections[1] = new serial::DummyConnection;

	mConnections[0]->connect(*mConnections[1]);
}

void DummyConnection::teardown(void)
{
	delete mConnections[0]; mConnections[0] = nullptr;
	delete mConnections[1]; mConnections[1] = nullptr;
}

void DummyConnection::define(void)
{

	UNIT_TEST("can construct dummy objects")
	{
	};

	UNIT_TEST("can close connection")
	{
		byte_t buf[] = { 0xFF, 0xFF, 0xFF };

		ASSERT_TRUE(mConnections[0]->isOpen());
		ASSERT_TRUE(mConnections[1]->isOpen());

		lib::Thread worker([this](void) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			mConnections[0]->close();
		});

		ASSERT_FAILURE(mConnections[0]->read(buf, sizeof(buf)), serial::Connection::ConnectionClosedException);
		ASSERT_FALSE(mConnections[0]->isOpen());
		ASSERT_FALSE(mConnections[1]->isOpen());
	};

	UNIT_TEST("can send some data")
	{
		send(mConnections[0], mConnections[1]);
		send(mConnections[1], mConnections[0]);
	};

	UNIT_TEST("works with threads")
	{
		const std::string cmp("Hello, World!");
		const size_t n = cmp.size();
		std::vector<byte_t> target(n);

		const byte_t *p = (const byte_t *) cmp.c_str();

		{
			lib::Thread reader([&target, this, n](void) {
				try
				{
					mConnections[0]->read(&target.front(), n);
				}
				MXT_CATCH_STRAY
			});

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			mConnections[1]->write(p, n - 5);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			mConnections[1]->write(p + n - 5, 5);
		}

		ASSERT_EACH_EQUALS(target, cmp);
	};

	UNIT_TEST("throws exception during read if closed in another thread")
	{
		lib::Thread t([this](void) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			delete mConnections[0]; mConnections[0] = nullptr;
		});

		byte_t buf[10];

		ASSERT_FAILURE(mConnections[0]->read(buf, sizeof(buf)), serial::Connection::ConnectionClosedException);
	};
}

}}}
