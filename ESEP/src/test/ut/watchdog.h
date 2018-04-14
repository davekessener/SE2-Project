#ifndef ESEP_TEST_UNIT_WATCHDOG_H
#define ESEP_TEST_UNIT_WATCHDOG_H

#include "test/unit/unit.h"

#include "lib/utils.h"
#include "lib/watchdog.h"
#include "serial/client.h"
#include "serial/connection.h"
#include "serial/dummy_connection.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class Watchdog : public TestSuite
			{
				typedef lib::Watchdog<serial::Client> wd_t;

				public:
					Watchdog( );
					void setup( );
					void teardown( );
					void define( );
				private:
					serial::DummyConnection *mConnections[2];
					wd_t *mWatchdog[2];
			};
		}
	}
}

#endif
