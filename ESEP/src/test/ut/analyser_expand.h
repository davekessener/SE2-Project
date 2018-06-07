#ifndef ESEP_TEST_UT_ANALYSEREXPAND_H
#define ESEP_TEST_UT_ANALYSEREXPAND_H

#include <vector>

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class AnalyserExpand : public unit::TestSuite
			{
				public:
					AnalyserExpand( );

				protected:
					void setup( ) override;
					void define( ) override;

				private:
					std::vector<std::pair<int, int>> mBuf, mCmp;
			};
		}
	}
}

#endif
