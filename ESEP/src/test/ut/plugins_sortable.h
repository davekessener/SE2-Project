#ifndef ESEP_TEST_UT_PLUGINSORTABLE_H
#define ESEP_TEST_UT_PLUGINSORTABLE_H

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class PluginSortable : public unit::TestSuite
			{
				public:
					PluginSortable( );

				protected:
					void define( ) override;

				private:
			};
		}
	}
}

#endif
