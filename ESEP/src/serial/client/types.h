#ifndef ESEP_SERIAL_CLIENT_BASE_H
#define ESEP_SERIAL_CLIENT_BASE_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <deque>
#include <stdexcept>

#include "serial/client.h"
#include "serial/connection.h"

#include "lib/byte_stream.h"
#include "lib/sync/container.h"

namespace esep
{
	namespace serial
	{
		namespace types
		{
			typedef std::unique_lock<std::mutex> lock_t;
			typedef uint8_t id_t;
			typedef lib::ByteStream buffer_t;
			typedef sync::Container<buffer_t> storage_t;

			MXT_DEFINE_E(BadPacketException);
			MXT_DEFINE_E(FailedPacketException);
			MXT_DEFINE_E(PacketDataOverflowException);
		}
	}
}

#endif
