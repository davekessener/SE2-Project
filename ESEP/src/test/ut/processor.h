#ifndef ESEP_TEST_UT_PROCESSOR_H
#define ESEP_TEST_UT_PROCESSOR_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class Processor : public unit::TestSuite
			{
				public:
					Processor( );

				protected:
					void define( );

				private:
			};
		}
	}
}

#endif
