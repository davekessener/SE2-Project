#ifndef ESEP_SERIAL_CLIENT_PACKET_SDP_H
#define ESEP_SERIAL_CLIENT_PACKET_SDP_H

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			class SmallData : public Base
			{
				static constexpr uint SIZE = 15;

				public:
				static constexpr type_t ID = 0x81A5;

				public:
					SmallData(id_t, const types::buffer_t&);
					virtual types::buffer_t serialize( );
					static packet_ptr deserialize(const types::buffer_t&);
				private:
					id_t mID;
					byte_t mData[SIZE];
					uint mLength;
			};
		}
	}
}

#endif
