#include <thread>
#include <chrono>
#include <atomic>

#include "test/ut/sync_container.h"

#include "lib/logger.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

SyncContainer::SyncContainer(void)
	: TestSuite("sync/Container")
	, mContainer(nullptr)
{
}

void SyncContainer::setup(void)
{
	mContainer = new container_t;
}

void SyncContainer::teardown(void)
{
	delete mContainer; mContainer = nullptr;
}

void SyncContainer::define(void)
{
	UNIT_TEST("can instantiate container")
	{
	};

	UNIT_TEST("can insert and remove from container")
	{
		mContainer->insert(0x33);
		mContainer->insert(0x70);

		ASSERT_EQUALS(mContainer->remove(), 0x33);
		ASSERT_EQUALS(mContainer->remove(), 0x70);
	};

	UNIT_TEST("it's empty after insertion/deletion")
	{
		ASSERT_EQUALS(mContainer->size(), 0u);
		ASSERT_TRUE(mContainer->empty());

		mContainer->insert(0);
		mContainer->insert(1);
		mContainer->insert(2);

		ASSERT_EQUALS(mContainer->size(), 3u);
		ASSERT_FALSE(mContainer->empty());

		mContainer->remove();
		mContainer->remove();
		mContainer->remove();

		ASSERT_EQUALS(mContainer->size(), 0u);
		ASSERT_TRUE(mContainer->empty());
	};

	UNIT_TEST("reading block on an empty container")
	{
		lib::Thread producer([this](void) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			mContainer->insert(0xAB);
		});

		ASSERT_TRUE(mContainer->empty());

		byte_t v = mContainer->remove();

		ASSERT_EQUALS(v, 0xAB);
		ASSERT_TRUE(mContainer->empty());
	};

	UNIT_TEST("throws InterruptedException if destroyed during read")
	{
		container_t *c = new container_t;

		lib::Thread t([&c](void) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			delete c;
			c = nullptr;
		});

		ASSERT_FAILURE(c->remove(), container_t::InterruptedException);

		ASSERT_FALSE(c);
	};
}

}}}
