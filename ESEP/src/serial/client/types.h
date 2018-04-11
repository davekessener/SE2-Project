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

#include "lib/sync/container.h"

namespace esep
{
	namespace serial
	{
		namespace types
		{
			typedef std::unique_lock<std::mutex> lock_t;
			typedef uint8_t id_t;
			typedef ByteStream buffer_t;
			typedef sync::Container<buffer_t> storage_t;

			struct BadPacketException : public std::exception { };
			struct ResetTriggeredException : public std::exception { };
			struct FailedPacketRead : public std::exception { };

			struct PacketDataOverflowException : public std::runtime_error
				{ PacketDataOverflowException(const std::string& s) : std::runtime_error(s) { } };
		}
	}
}

#endif
