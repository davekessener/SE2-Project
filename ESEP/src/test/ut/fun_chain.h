#ifndef ESEP_TEST_UT_FUNCHAIN_H
#define ESEP_TEST_UT_FUNCHAIN_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class FunChain : public unit::TestSuite
			{
				public:
					FunChain( );

				protected:
					void define( );
			};
		}
	}
}

#endif
