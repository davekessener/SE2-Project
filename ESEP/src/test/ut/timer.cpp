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

		ASSERT_APPROX_EQUALS(mCounter, 6u);
		ASSERT_EQUALS(mCounter, c);
	};

	UNIT_TEST("calls timer once")
	{
		auto t = mTimer->registerCallback(mIncrementer, 0, 0);

		mTimer->sleep(50);

		ASSERT_APPROX_EQUALS(mCounter, 1u);
	};

	UNIT_TEST("calls timer periodically")
	{
		auto t = mTimer->registerCallback(mIncrementer, 0, 10);

		mTimer->sleep(95);

		ASSERT_APPROX_EQUALS(mCounter, 10u);
	};

	UNIT_TEST("can unregister timer")
	{
		auto t = mTimer->registerCallback(mIncrementer, 0, 10);

		mTimer->sleep(55);
		mTimer->unregisterCallback(t);

		auto c = mCounter;

		mTimer->sleep(50);

		ASSERT_APPROX_EQUALS(mCounter, 6u);
		ASSERT_EQUALS(mCounter, c);
	};

	UNIT_TEST("can execute async timer")
	{
		auto t = mTimer->registerAsync(mIncrementer, 0, 10);

		mTimer->sleep(35);

		ASSERT_APPROX_EQUALS(mCounter, 4u);
	};

	UNIT_TEST("can unregister async timer")
	{
		{
			auto t = mTimer->registerAsync(mIncrementer, 0, 10);

			mTimer->sleep(55);
		}

		auto c = mCounter;

		mTimer->sleep(25);

		ASSERT_APPROX_EQUALS(mCounter, 6u);
		ASSERT_EQUALS(mCounter, c);
	};

	UNIT_TEST("can handle async deletion before thread initialization")
	{
		{
			auto t = mTimer->registerAsync(mIncrementer, 5, 10);
		}

		mTimer->sleep(30);

		ASSERT_APPROX_EQUALS(mCounter, 0u);
	};

	UNIT_TEST("can handle double deletion")
	{
		auto t = mTimer->registerCallback(mIncrementer, 0, 10);

		mTimer->sleep(25);

		t.reset();

		mTimer->unregisterCallback(t);

		mTimer->sleep(40);

		mTimer->unregisterCallback(t);

		ASSERT_APPROX_EQUALS(mCounter, 3u);
	};

	UNIT_TEST("can handle async double deletion")
	{
		auto t = mTimer->registerAsync(mIncrementer, 0, 10);

		mTimer->sleep(25);

		t.reset();

		mTimer->unregisterCallback(t);

		mTimer->sleep(40);

		mTimer->unregisterCallback(t);

		ASSERT_APPROX_EQUALS(mCounter, 3u);
	};

	UNIT_TEST("calls async exactly once")
	{
		auto t = mTimer->registerAsync(mIncrementer, 10);

		mTimer->sleep(5);

		ASSERT_EQUALS(mCounter, 0u);

		mTimer->sleep(15);

		ASSERT_EQUALS(mCounter, 1u);

		mTimer->sleep(20);

		ASSERT_EQUALS(mCounter, 1u);
	};
}

}}}
