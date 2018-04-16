#ifndef SRC_TEST_FT_SERIAL_CONNECTION_H_
#define SRC_TEST_FT_SERIAL_CONNECTION_H_

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
