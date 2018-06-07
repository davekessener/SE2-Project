#ifndef ESEP_TEST_UT_OVERFLOWBUFFER_H
#define ESEP_TEST_UT_OVERFLOWBUFFER_H

#include "test/unit/unit.h"

#include "lib/overflow_buffer.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class OverflowBuffer : public unit::TestSuite
			{
				typedef lib::OverflowBuffer<int, 10> buf_t;

				public:
					OverflowBuffer( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					buf_t *mBuf;
			};
		}
	}
}

#endif
