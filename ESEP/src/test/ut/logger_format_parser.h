#ifndef ESEP_TEST_UT_LOGGERFORMATPARSER_H
#define ESEP_TEST_UT_LOGGERFORMATPARSER_H

#include "lib/log/format.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class LoggerFormatParser : public unit::TestSuite
			{
				public:
					LoggerFormatParser( );
				protected:
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
