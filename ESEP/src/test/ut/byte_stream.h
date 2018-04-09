#ifndef ESEP_TEST_UNIT_BYTESTREAM_H
#define ESEP_TEST_UNIT_BYTESTREAM_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class ByteStream : public TestSuite
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
