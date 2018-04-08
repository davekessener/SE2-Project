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
					Resetter(InOut_Connection& c) : mConnection(d) { }
					void send( );
					void respond( );
				private:
					InOut_Connection& mConnection;
			};
		}
	}
}

#endif
