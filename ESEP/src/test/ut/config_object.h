
#ifndef ESEP_TEST_UT_CONFIG_OBJECT_H
#define ESEP_TEST_UT_CONFIG_OBJECT_H

#include "lib/utils.h"

#include "base/config_object.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class ConfigObject : public TestSuite
			{
				typedef base::ConfigObject config_t;

				public:
					ConfigObject( );
				protected:
					void setup( );
					void teardown( );
					void define( );
				private:
					config_t *mConfig;
			};
		}
	}
}



#endif
