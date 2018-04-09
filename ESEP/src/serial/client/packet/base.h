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
				AP_ERR = 0x3613
			};

			class Base
			{
				public:
					virtual ~Base( ) { }
					virtual types::buffer_t serialize( ) = 0;
				private:
			};

			typedef std::shared_ptr<Base> packet_ptr;
		}
	}
}

#endif
