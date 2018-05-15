#ifndef ESEP_SERIAL_CLIENT_PACKET_AP_H
#define ESEP_SERIAL_CLIENT_PACKET_AP_H

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			template<Type T>
			class Answer : public Base
			{
				public:
					Answer(types::id_t id) : Base(id) { }
					virtual Type getType( ) const { return T; }
					virtual types::buffer_t serialize( );
					static packet_ptr deserialize(modules::In_Connection&);
			};

			template<Type T>
			types::buffer_t Answer<T>::serialize(void)
			{
				types::buffer_t b;

				b << static_cast<uint16_t>(T) << getID();

				byte_t checksum = lib::checksum(b);

				b << checksum;

				return b;
			}

			template<Type T>
			packet_ptr Answer<T>::deserialize(modules::In_Connection& c)
			{
				types::buffer_t b;

				b << static_cast<uint16_t>(T);

				c.read(b, sizeof(types::id_t) + sizeof(byte_t));

				if(lib::checksum(b) != 0)
				{
					MXT_THROW_EX(types::FailedPacketException);
				}

				uint16_t type;
				id_t id;

				b >> type >> id;

				return packet_ptr(new Answer<T>(id));
			}
		}
	}
}

#endif
