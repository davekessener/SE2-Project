#ifndef ESEP_SERIAL_CLIENT_MODULE_DESERIALIZE_H
#define ESEP_SERIAL_CLIENT_MODULE_DESERIALIZE_H

#include "serial/client/types.h"
#include "serial/client/m_connection.h"

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Deserializer
			{
				public:
					Deserializer(In_Connection& c) : mConnection(c) { }
					packet::packet_ptr receive( );
				private:
					In_Connection& mConnection;
			};
		}
	}
}

#endif
