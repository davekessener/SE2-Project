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
			private:
			typedef std::unique_ptr<Client> client_ptr;
			typedef sync::Container<Client::buffer_t> container_t;

			enum class Packet : uint8_t
			{
				DATA = 0,
				WATCHDOG = 0xFF
			};

			public:
			static constexpr uint DEFAULT_TIMEOUT = 100;

			public:
				Watchdog(client_ptr, uint = DEFAULT_TIMEOUT);
				virtual ~Watchdog( );
				virtual void write(const Client::buffer_t&);
				virtual Client::buffer_t read( );
				virtual bool connected( ) const { return mIsActive.load(); }
			private:
				void sendPacket(const Client::buffer_t&);
				void sendWatchdog( )
					{ Client::buffer_t b; b.push_back(static_cast<byte_t>(Packet::WATCHDOG)); sendPacket(b); }

			private:
				client_ptr mClient;
				timer::Manager mTimer;
				std::atomic<uint64_t> mLastRead, mLastWrite;
				uint mTimeout;
				lib::Thread mReaderThread;
				container_t mReadBuf;
				std::atomic<bool> mTimedOut, mRunning, mIsActive;
		};
	}
}

#endif
