#ifndef ESEP_SERIAL_WATCHDOG_H
#define ESEP_SERIAL_WATCHDOG_H

#include <atomic>

#include "lib/utils.h"
#include "lib/timer.h"
#include "lib/sync/container.h"

#include "serial/client.h"

namespace esep
{
	namespace serial
	{
		class Watchdog : public Client
		{
			typedef std::unique_ptr<Client> client_ptr;

			enum class Packet : uint8_t
			{
				DATA = 0,
				WATCHDOG = 0xFF
			};

			public:
				Watchdog(client_ptr, uint);
				virtual ~Watchdog( );
				virtual void write(const Client::buffer_t& o)
					{ auto b(o); b.insert(b.begin(), static_cast<byte_t>(Packet::DATA)); sendPacket(b); }
				virtual Client::buffer_t read( ) { return mReadBuf.remove(); }
			private:
				void sendPacket(const Client::buffer_t&);

			private:
				client_ptr mClient;
				lib::Timer::Class::id_t mTimerID;
				uint64_t mLastRead, mLastWrite;
				uint mTimeout;
				std::thread mReaderThread;
				sync::Container<Client::buffer_t> mReadBuf;
				std::atomic<bool> mTimedOut, mRunning;
		};
	}
}

#endif
