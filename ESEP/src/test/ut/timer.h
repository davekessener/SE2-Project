#ifndef ESEP_TEST_UNIT_TIMER_H
#define ESEP_TEST_UNIT_TIMER_H

#include <functional>

#include "test/unit/unit.h"

#include "lib/timer.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class Timer : public TestSuite
			{
				typedef std::function<void(void)> cb_fn;

				public:
					Timer( );
				protected:
					void setup( );
					void define( );
				private:
					lib::Timer::Class *mTimer;
					uint mCounter;
					cb_fn mIncrementer;
			};
		}
	}
}

#endif
