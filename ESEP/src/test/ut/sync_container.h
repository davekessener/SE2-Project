#ifndef ESEP_TEST_UT_SYNCCONTAINER_H
#define ESEP_TEST_UT_SYNCCONTAINER_H

#include "lib/utils.h"
#include "lib/sync/container.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class SyncContainer : public unit::TestSuite
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
