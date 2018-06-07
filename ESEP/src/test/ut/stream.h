#ifndef ESEP_TEST_UT_STREAM_H
#define ESEP_TEST_UT_STREAM_H

#include <vector>

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class Stream : public unit::TestSuite
			{
				public:
					Stream( );

				protected:
					void setup( ) override;
					void define( ) override;

				private:
					std::vector<double> mBuf;
			};
		}
	}
}

#endif
