#ifndef ESEP_TEST_FUNCTIONAL_HAL_H
#define ESEP_TEST_FUNCTIONAL_HAL_H

#include "test/ft/hal/tester.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			void testHAL( )
			{
				HALTester tester;

				tester.run();
			}
		}
	}
}

#endif
