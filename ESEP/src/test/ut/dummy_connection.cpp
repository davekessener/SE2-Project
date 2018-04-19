#ifdef ESEP_TEST

#include <chrono>
#include <vector>

#include "test/ut/dummy_connection.h"
#include "test/unit/assertions.h"

#include "serial/dummy_connection.h"

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

		byte_t *p1 = (byte_t *) cmp.c_str();
		byte_t *p2 = (byte_t *) &target.front();
		int some_var = 0;
		size_t l1, l2;

		{
			lib::Thread reader([&target, this, n](void) {
				try
				{
					mConnections[0]->read(&target.front(), n);
				}
				MXT_CATCH_STRAY
			});

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			mConnections[1]->write(p1, n - 5);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			mConnections[1]->write(p1 + n - 5, 5);
		}

		do {
			auto& aobj(target);
			auto& bobj(cmp);
			MXT_LOG(lib::stringify("Size of cmp is ", bobj.size()));
//			auto aobj(lib::wrap_container(target));
//			auto bobj(lib::wrap_container(cmp));
			auto i1 = std::begin(aobj), i2 = std::end(aobj);
			auto j1 = std::begin(bobj);
			if((l1 = aobj.size()) != (l2 = bobj.size()))
				MXT_THROW_E(std::logic_error, "Container have different sizes: ", aobj.size(), " and ", bobj.size(), "!");
			for(size_t i = 0 ; i1 != i2 ; ++i1, ++j1, ++i) {
				if(*i1 != *j1)
					MXT_THROW_E(std::logic_error, "Elements @", i, " differ: Expected ", *j1, " but got ", *i1, " instead!");
			} } while(0);

		some_var = 1;

//		ASSERT_EQUALS(target.size(), cmp.size());
//		ASSERT_EACH_EQUALS(target, cmp);
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

#endif
