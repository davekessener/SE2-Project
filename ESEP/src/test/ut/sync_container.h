#ifndef ESEP_TEST_UNIT_SYNCCONTAINER_H
#define ESEP_TEST_UNIT_SYNCCONTAINER_H

#include "lib/utils.h"
#include "lib/sync/container.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class SyncContainer : public TestSuite
			{
				typedef sync::Container<byte_t> container_t;

				public:
					SyncContainer( );
				protected:
					void setup( );
					void teardown( );
					void define( );
				private:
					container_t *mContainer;
			};
		}
	}
}

#endif
