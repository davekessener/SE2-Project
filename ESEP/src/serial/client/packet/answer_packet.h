#ifndef ESEP_SERIAL_CLIENT_PACKET_AP_H
#define ESEP_SERIAL_CLIENT_PACKET_AP_H

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			class Answer : public Base
			{
				public:
				enum class Type : uint16_t
				{
					OK = 0x187C,
					ERR = 0x3613
				};

				public:
					Answer(Type t, types::id_t id) : mType(t), mID(id) { }
					virtual types::buffer_t serialize( );
					static packet_ptr deserialize(const types::buffer_t&);
				private:
					Type mType;
					types::id_t mID;
			};
		}
	}
}

#endif
