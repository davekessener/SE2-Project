#ifndef ESEP_TEST_UNIT_EMPPARSER_H
#define ESEP_TEST_UNIT_EMPPARSER_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class EMPParser : public TestSuite
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
