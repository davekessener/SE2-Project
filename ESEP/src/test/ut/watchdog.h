#ifndef ESEP_TEST_UNIT_WATCHDOG_H
#define ESEP_TEST_UNIT_WATCHDOG_H

#include "test/unit/unit.h"

#include "lib/utils.h"
#include "serial/client.h"
#include "serial/watchdog.h"
#include "serial/dummy_connection.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class Watchdog : public TestSuite
			{
				public:
					Watchdog( );
					void setup( );
					void teardown( );
					void define( );
				private:
					serial::Client *mWatchdog[2];
					serial::DummyConnection *mConnections[2];
			};
		}
	}
}

#endif
