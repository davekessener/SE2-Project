#ifndef ESEP_TEST_UT_CRC32_H
#define ESEP_TEST_UT_CRC32_H

#include "lib/utils.h"
#include "lib/crc.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class CRC32 : public TestSuite
			{
				typedef lib::crc::Generator<uint32_t> gen_t;

				public:
					CRC32( );
				protected:
					void setup( );
					void teardown( );
					void define( );
				private:
					gen_t *mCRCGen;
			};
		}
	}
}

#endif
