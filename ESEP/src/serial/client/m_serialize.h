#ifndef ESEP_SERIAL_CLIENT_MODULE_SERIALIZE_H
#define ESEP_SERIAL_CLIENT_MODULE_SERIALIZE_H

#include "serial/client/types.h"
#include "serial/client/m_connection.h"

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Serializer
			{
				public:
					Serializer(Out_Connection& c) : mConnection(c) { }
					void send(packet::packet_ptr);
				private:
					Out_Connection& mConnection;
			};
		}
	}
}

#endif
