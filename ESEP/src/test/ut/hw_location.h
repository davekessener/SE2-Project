#ifndef ESEP_TEST_UT_HWLOCATION_H
#define ESEP_TEST_UT_HWLOCATION_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class HWLocation : public unit::TestSuite
			{
				public:
					HWLocation( );
				protected:
					void define( ) override;
				private:
			};
		}
	}
}

#endif
