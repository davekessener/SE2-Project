#ifndef ESEP_SERIAL_CLIENT_MODULE_WRITE_H
#define ESEP_SERIAL_CLIENT_MODULE_WRITE_H

#include "serial/client/types.h"
#include "serial/client/m_serialize.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Writer
			{
				public:
					Writer(Serializer& c) : mConnection(c) { }
					void put(const types::buffer_t& o) { mStorage.insert(o); }
				private:
					Serializer& mConnection;
					types::storage_t mStorage;
			};
		}
	}
}

#endif
