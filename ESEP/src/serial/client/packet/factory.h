#ifndef ESEP_SERIAL_CLIENT_PACKET_FACTORY_H
#define ESEP_SERIAL_CLIENT_PACKET_FACTORY_H

#include "serial/client/packet/base.h"

#include "serial/client/m_connection.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			template<uint16_t ID, packet_ptr (*F)()
			class Factory
		}
	}
}

#endif
