#ifndef ESEP_TEST_UT_BYTESTREAM_H
#define ESEP_TEST_UT_BYTESTREAM_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class ByteStream : public unit::TestSuite
			{
				public:
					ByteStream( );
				protected:
					void define( );
			};
		}
	}
}

#endif
