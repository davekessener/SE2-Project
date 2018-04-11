#ifndef ESEP_SERIAL_CLIENT_MODULE_READ_H
#define ESEP_SERIAL_CLIENT_MODULE_READ_H

#include <array>

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
					Reader(Writer& c) : mConnection(c), mHistorySize(0) { }
					void process(packet::packet_ptr);
					types::buffer_t get( ) { return mStorage.remove(); }
				private:
					void processDataPacket(packet::packet_ptr);
					bool recordPacket(packet::packet_ptr);
				private:
					Writer& mConnection;
					types::storage_t mStorage;
					types::buffer_t mChain;
					lib::RestrictedList<packet::packet_ptr, N> mHistory;
			};
		}
	}
}

#endif
