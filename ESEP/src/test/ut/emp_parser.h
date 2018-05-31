#ifndef ESEP_TEST_UT_EMPPARSER_H
#define ESEP_TEST_UT_EMPPARSER_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class EMPParser : public unit::TestSuite
			{
				public:
					EMPParser( );
				protected:
					void define( ) override;
				private:
			};
		}
	}
}

#endif
