#ifndef ESEP_SERIAL_CLIENT_MODULE_READ_H
#define ESEP_SERIAL_CLIENT_MODULE_READ_H

#include "serial/client/types.h"
#include "serial/client/m_serialize.h"

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Reader
			{
				public:
					Reader(Serializer& c) : mConnection(c) { }
					void process(packet::packet_ptr);
					types::buffer_t get( ) { return mStorage.remove(); }
				private:
					Serializer& mConnection;
					types::storage_t mStorage;
			};
		}
	}
}

#endif
