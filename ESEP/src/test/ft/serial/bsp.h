#ifndef ESEP_TEST_FT_SERIALCONNECTION_H
#define ESEP_TEST_FT_SERIALCONNECTION_H

#include <memory>

#include "serial/connection.h"
#include "serial/client.h"
#include "serial/actual_connection.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			class SerialConnectionTest
			{
				typedef std::unique_ptr<serial::Connection> connection_ptr;
				typedef std::unique_ptr<serial::Client> client_ptr;
				typedef serial::Client::buffer_t buffer_t;
				typedef serial::Connection::ConnectionClosedException ConnectionClosedException;

				public:
					SerialConnectionTest();
					~SerialConnectionTest();
					void run();
				private:
					client_ptr mClient;
			};
		}
	}
}

#endif
