#ifndef ESEP_SERIAL_CLIENT_MODULE_DESERIALIZE_H
#define ESEP_SERIAL_CLIENT_MODULE_DESERIALIZE_H

#include "lib/factory.h"

#include "serial/client/types.h"
#include "serial/client/m_connection.h"

#include "serial/client/packet/base.h"
#include "serial/client/packet/data_packet.h"
#include "serial/client/packet/answer_packet.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Deserializer
			{
				struct TypeComparator
				{
					inline uint numBits(uint16_t v) { uint n = 0; while(v) { if(v & 1) ++n; v >>= 1; } return n; }
					bool operator()(packet::Type t1, packet::Type t2)
					{
						return numBits(static_cast<uint16_t>(t1) ^ static_cast<uint16_t>(t2)) <= 1;
					}
				};

				typedef packet::packet_ptr (*generator_type)(In_Connection&);

				template<packet::Type K, generator_type F>
				using Entry = lib::Generator<packet::Type, K, generator_type, F>;

				template<packet::Type K>
				using DataPacket = Entry<K, &packet::Data<K>::deserialize>;

				template<packet::Type K>
				using AnswerPacket = Entry<K, &packet::Answer<K>::deserialize>;

				typedef lib::Factory<tml::MakeTypeList<
					DataPacket<packet::Type::SDP>,
					DataPacket<packet::Type::MDP>,
					DataPacket<packet::Type::LDP>,
					AnswerPacket<packet::Type::AP_OK>,
					AnswerPacket<packet::Type::AP_ERR>
				>, TypeComparator, types::BadPacketException> factory_t;

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
