#ifndef ESEP_TEST_UT_EMPPLAYBACK_H
#define ESEP_TEST_UT_EMPPLAYBACK_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class EMPPlayback : public unit::TestSuite
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
