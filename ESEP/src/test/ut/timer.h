#ifndef ESEP_TEST_UNIT_TIMER_H
#define ESEP_TEST_UNIT_TIMER_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class Timer : public TestSuite
			{
				public:
					Timer( );
				protected:
					void define( );
				private:
			};
		}
	}
}

#endif
