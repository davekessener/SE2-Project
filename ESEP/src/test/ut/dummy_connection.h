#ifndef ESEP_TEST_UNIT_DUMMYCONNECTION_H
#define ESEP_TEST_UNIT_DUMMYCONNECTION_H

#include <deque>

#include "serial/dummy_connection.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class DummyConnection : public TestSuite
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
