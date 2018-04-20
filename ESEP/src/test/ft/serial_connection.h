#ifndef ESEP_TEST_FT_SERIALCONNECTION_H
#define ESEP_TEST_FT_SERIALCONNECTION_H

#include "serial/connection.h"
#include "serial/client.h"
#include "serial/actual_connection.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			class SerialConnection
			{
				public:
					SerialConnection();
					~SerialConnection();
					void run();
				private:
					serial::Client *mClient;
					serial::Connection *mConnection;
			};
		}
	}
}

#endif
