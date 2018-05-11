#ifndef ESEP_TEST_UNIT_HWLOCATION_H
#define ESEP_TEST_UNIT_HWLOCATION_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class HWLocation : public TestSuite
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
