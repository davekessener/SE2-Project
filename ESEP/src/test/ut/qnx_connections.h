#ifndef ESEP_TEST_UNIT_QNXCONNECTIONS_H
#define ESEP_TEST_UNIT_QNXCONNECTIONS_H

#include "lib/utils.h"
#include "qnx/channel.h"
#include "qnx/connection.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class QNXConnections : public TestSuite
			{
				public:
					QNXConnections( );
					void setup( );
					void teardown( );
					void define( );
				private:
					qnx::Channel *mChannel;
					qnx::Connection mConnection;
			};
		}
	}
}

#endif
