#ifndef ESEP_SERIAL_CLIENT_PACKET_TRAITS_H
#define ESEP_SERIAL_CLIENT_PACKET_TRAITS_H

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			template<Type T> struct Traits;

			template<>
			struct Traits<Type::SDP>
			{
				static constexpr uint Size = 15;
				typedef uint8_t crc_type;
				typedef uint8_t size_type;
				static crc_type calculateCRC(uint32_t crc32)
				{
					uint8_t *p = reinterpret_cast<uint8_t *>(&crc32);

					return p[0] ^ p[1] ^ p[2] ^ p[3];
				}
			};

			template<>
			struct Traits<Type::MDP>
			{
				static constexpr uint Size = 64;
				typedef uint16_t crc_type;
				typedef uint8_t size_type;
				static crc_type calculateCRC(uint32_t crc32)
				{
					uint16_t *p = reinterpret_cast<uint16_t *>(&crc32);

					return p[0] ^ p[1];
				}
			};

			template<>
			struct Traits<Type::LDP>
			{
				static constexpr uint Size = 512;
				typedef uint32_t crc_type;
				typedef uint16_t size_type;
				static crc_type calculateCRC(uint32_t crc32) { return crc32; }
			};
		}
	}
}

#endif
