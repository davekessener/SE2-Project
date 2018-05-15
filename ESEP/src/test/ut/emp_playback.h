#ifndef ESEP_TEST_UNIT_EMPPLAYBACK_H
#define ESEP_TEST_UNIT_EMPPLAYBACK_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class EMPPlayback : public TestSuite
			{
				public:
					EMPPlayback( );

				protected:
					void define( ) override;
			};
		}
	}
}

#endif
