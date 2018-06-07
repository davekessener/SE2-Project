#ifndef ESEP_TEST_UT_SERIALCLIENT_H
#define ESEP_TEST_UT_SERIALCLIENT_H

#include "lib/utils.h"
#include "serial/connection.h"
#include "serial/client.h"
#include "serial/utils/dummy_connection.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class SerialClient : public unit::TestSuite
			{
				public:
					SerialClient( );
					void setup( );
					void teardown( );
					void define( );
				private:
					serial::Client *mClients[2];
					serial::DummyConnection *mConnections[2];
			};
		}
	}
}

#endif
