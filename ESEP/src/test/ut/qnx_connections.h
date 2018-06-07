#ifndef ESEP_TEST_UT_QNXCONNECTIONS_H
#define ESEP_TEST_UT_QNXCONNECTIONS_H

#include "lib/utils.h"
#include "qnx/channel.h"
#include "qnx/connection.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class QNXConnections : public unit::TestSuite
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
