#ifndef ESEP_TEST_UT_ANY_H
#define ESEP_TEST_UT_ANY_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class Any : public unit::TestSuite
			{
				public:
					Any( );

				protected:
					void define( ) override;
			};
		}
	}
}

#endif
