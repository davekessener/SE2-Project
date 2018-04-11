#ifndef ESEP_SERIAL_CLIENT_MODULE_WRITE_H
#define ESEP_SERIAL_CLIENT_MODULE_WRITE_H

#include <mutex>

#include "lib/utils.h"
#include "lib/sync/container.h"

#include "serial/client/types.h"
#include "serial/client/m_serialize.h"

#include "serial/client/packet/answer_packet.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Writer
			{
				typedef std::unique_lock<std::mutex> lock_t;

				public:
					Writer(Serializer&);
					void put(const types::buffer_t&);
					void acknowledge(types::id_t, packet::Type);
					void reset( );
					void send(packet::packet_ptr);
				private:
					struct Impl;

					Impl *pImpl;
					std::mutex mMutex;
			};
		}
	}
}

#endif
