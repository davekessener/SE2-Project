#ifndef ESEP_SERIAL_WATCHDOG_H
#define ESEP_SERIAL_WATCHDOG_H

#include <atomic>
#include <chrono>

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
			typedef std::chrono::time_point<std::chrono::system_clock> time_t;

			enum class Packet : uint8_t
			{
				DATA = 0,
				WATCHDOG = 0xFF
			};

			public:
			static constexpr uint DEFAULT_TIMEOUT = 300;

			MXT_DEFINE_E(CantSendException);

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
				uint64_t elapsed( ) const
					{ return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mStartTime).count() + 1; }

			private:
				client_ptr mClient;
				timer::Manager mTimer;
				std::atomic<uint64_t> mLastRead, mLastWrite;
				const time_t mStartTime;
				uint mTimeout;
				lib::Thread mReaderThread;
				container_t mReadBuf;
				std::atomic<bool> mTimedOut, mRunning, mIsActive;
		};
	}
}

#endif
