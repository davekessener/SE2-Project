#ifndef ESEP_SERIAL_CLIENT_PACKET_BASE_H
#define ESEP_SERIAL_CLIENT_PACKET_BASE_H

#include <memory>

#include "serial/client/types.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			enum class Type : uint16_t
			{
				SDP    = 0x81A5,
				MDP    = 0x4296,
				LDP    = 0x42C9,
				AP_OK  = 0x187C,
				AP_ERR = 0x3613,
				RESET  = 0x70B1
			};

			class Base
			{
				public:
				typedef types::id_t id_t;

				public:
					Base(id_t id) : mID(id) { }
					virtual ~Base( ) { }
					virtual types::buffer_t serialize( ) = 0;
					virtual Type getType( ) const = 0;
					id_t getID( ) const { return mID; }
				private:
					const id_t mID;
			};

			typedef std::shared_ptr<Base> packet_ptr;
		}
	}
}

#endif
