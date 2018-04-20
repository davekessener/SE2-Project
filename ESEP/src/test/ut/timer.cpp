#include "test/ut/timer.h"

#include "lib/logger.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

Timer::Timer(void)
	: TestSuite("Timer")
{
	mTimer = &lib::Timer::instance();
	mCounter = 0;
	mIncrementer = [this](void) { ++mCounter; };
}

void Timer::setup(void)
{
	mCounter = 0;
}

void Timer::define(void)
{
	UNIT_TEST("automatic timer deletion works")
	{
		{
			auto t = mTimer->registerCallback(mIncrementer, 0, 10);

			mTimer->sleep(55);
		}

		auto c = mCounter;

		mTimer->sleep(25);

		ASSERT_APPROX_EQUALS(mCounter, 5u);
		ASSERT_EQUALS(mCounter, c);
	};

	UNIT_TEST("calls timer once")
	{
		auto t = mTimer->registerCallback(mIncrementer, 0, 0);

		mTimer->sleep(50);

		ASSERT_EQUALS(mCounter, 1u);
	};

	UNIT_TEST("calls timer periodically")
	{
		auto t = mTimer->registerCallback(mIncrementer, 0, 10);

		mTimer->sleep(105);

		ASSERT_APPROX_EQUALS(mCounter, 10u);
	};

	UNIT_TEST("can unregister timer")
	{
		auto t = mTimer->registerCallback(mIncrementer, 0, 10);

		mTimer->sleep(55);
		mTimer->unregisterCallback(t);

		auto c = mCounter;

		mTimer->sleep(50);

		ASSERT_APPROX_EQUALS(mCounter, 5u);
		ASSERT_EQUALS(mCounter, c);
	};
}

}}}
