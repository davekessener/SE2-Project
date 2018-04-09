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
					Answer(Type t, types::id_t id) : mType(t), mID(id) { }
					virtual types::buffer_t serialize( );
					static packet_ptr deserialize(modules::In_Connection&);
				private:
					Type mType;
					types::id_t mID;
			};
		}
	}
}

#endif
