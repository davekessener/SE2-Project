#ifndef ESEP_TEST_UT_LOGGERFORMATPARSER_H
#define ESEP_TEST_UT_LOGGERFORMATPARSER_H

#include "test/unit/unit.h"
#include "lib/log/format.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class LoggerFormatParser : public TestSuite
			{
				public:
					LoggerFormatParser( );
					void setup( );
					void teardown( );
					void define( );
				private:
					log::format::Parser *uut;
			};
		}
	}
}

#endif
