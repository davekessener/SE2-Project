#ifndef ESEP_TEST_FN_PLUGINS_ANALYSER_H
#define ESEP_TEST_FN_PLUGINS_ANALYSER_H

#include "lib/utils.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace plugins
			{
				class Analyser
				{
					public:
						Analyser( );
						~Analyser( );
						void run( );
					private:
						static uint16_t RAW_DATA(size_t);
						static size_t N_DATA( );
				};
			}
		}
	}
}

#endif
