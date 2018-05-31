#ifndef ESEP_TEST_UT_TESTHAL_H
#define ESEP_TEST_UT_TESTHAL_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class TestHAL : public unit::TestSuite
			{
				public:
					TestHAL( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					uint mCounter;
			};
		}
	}
}

#endif
