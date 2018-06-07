#ifndef ESEP_TEST_UT_EMPTOKENIZER_H
#define ESEP_TEST_UT_EMPTOKENIZER_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class EMPTokenizer : public unit::TestSuite
			{
				public:
					EMPTokenizer( );

				protected:
					void define( ) override;

				private:
			};
		}
	}
}

#endif
