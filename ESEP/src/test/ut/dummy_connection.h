#ifndef ESEP_TEST_UT_DUMMYCONNECTION_H
#define ESEP_TEST_UT_DUMMYCONNECTION_H

#include <deque>

#include "serial/utils/dummy_connection.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class DummyConnection : public unit::TestSuite
			{
				public:
					DummyConnection( );
					void setup( );
					void teardown( );
					void define( );
				private:
					serial::DummyConnection *mConnections[2];
			};
		}
	}
}

#endif
