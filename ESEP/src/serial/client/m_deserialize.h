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
				static packet::packet_ptr respondToReset(In_Connection&) { throw types::ResetTriggeredException(); }

				struct TypeComparator
				{
					inline uint numBits(uint16_t v) { uint n = 0; while(v) { if(v & 1) ++n; v >>= 1; } return n; }
					bool operator()(Type t1, Type t2)
					{
						return numBits(static_cast<uint16_t>(t1) ^ static_cast<uint16_t>(t2)) <= 1;
					}
				};

				typedef packet::packet_ptr (*generator_type)(In_Connection&);

				template<Type K, template <Type> class T>
				using Entry = lib::Generator<Type, K, generator_type, &T<K>::deserialize>;

				template<Type K>
				using DataPacket = Entry<K, packet::Data>;

				template<Type K>
				using AnswerPacket = Entry<K, packet::Answer>;

				typedef lib::Factory<tml::MakeTypeList<
					DataPacket<Type::SDP>,
					DataPacket<Type::MDP>,
					DataPacket<Type::LDP>,
					AnswerPacket<Type::ACK_OK>,
					AnswerPacket<Type::ACK_ERR>,
					lib::Generator<Type, Type::RESET, generator_type, &respondToReset>
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
