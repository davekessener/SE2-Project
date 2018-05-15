#ifndef ESEP_TEST_UNIT_EMPTOKENIZER_H
#define ESEP_TEST_UNIT_EMPTOKENIZER_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class EMPTokenizer : public TestSuite
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
