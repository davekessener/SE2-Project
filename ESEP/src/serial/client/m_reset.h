#ifndef ESEP_SERIAL_CLIENT_MODULE_RESET_H
#define ESEP_SERIAL_CLIENT_MODULE_RESET_H

#include "serial/client/m_connection.h"
#include "types.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Resetter
			{
				public:
					Resetter(InOut_Connection& c) : mConnection(c) { }
					void handle( ) { send(); receive(); }
				private:
					void send( );
					void receive( );
				private:
					InOut_Connection& mConnection;
			};
		}
	}
}

#endif
