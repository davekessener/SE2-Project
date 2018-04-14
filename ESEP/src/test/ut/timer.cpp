#include "test/ut/timer.h"

#include "lib/timer.h"
#include "lib/logger.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

Timer::Timer(void)
	: TestSuite("Timer")
{
}

void Timer::define(void)
{
	UNIT_TEST("call timer periodically")
	{
		auto& timer(lib::Timer::instance());

		uint c = 1;

		auto diff = [](uint a, uint b) { return (a > b) ? a - b : b - a; };

		auto cb = [&c](void) -> bool {
			if(!c)
			{
				return false;
			}
			else
			{
				++c;

				return true;
			}
		};

		timer.registerCallback(cb, 0, 10);

		timer.sleep(105);

		ASSERT_TRUE(diff(c, 11) <= 1);

		c = 0;
	};
}

}}}
