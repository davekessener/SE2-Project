#ifndef ESEP_SERIAL_CLIENT_MODULE_READ_H
#define ESEP_SERIAL_CLIENT_MODULE_READ_H

#include <array>

#include "lib/restricted_list.h"

#include "serial/client/types.h"
#include "serial/client/m_write.h"

#include "serial/client/packet/base.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Reader
			{
				static constexpr uint N = 10;

				public:
					Reader(Writer& c) : mConnection(c) { }
					void process(packet::packet_ptr);
					types::buffer_t get( );
					void reset( );
				private:
					void processDataPacket(packet::packet_ptr);
					bool recordPacket(packet::packet_ptr);
				private:
					Writer& mConnection;
					lib::RestrictedList<packet::packet_ptr, N> mHistory;
					types::storage_t mStorage;
					types::buffer_t mChain;
					packet::packet_ptr mLastAck;
			};
		}
	}
}

#endif
