#include "serial/watchdog.h"

#include "lib/logger.h"

namespace esep { namespace serial {

Watchdog::Watchdog(client_ptr c, uint t)
	: mClient(std::move(c))
	, mTimeout(t)
{
	mLastRead = mLastWrite = lib::Timer::instance().elapsed();
	mTimedOut = false;
	mRunning = true;

	auto timer = [this](void) {
		if(mRunning.load())
		try
		{
			auto e = lib::Timer::instance().elapsed();

			if(e - mLastWrite > mTimeout / 2)
			{
				Client::buffer_t b;

				b.push_back(static_cast<byte_t>(Packet::WATCHDOG));

				sendPacket(b);
			}
		}
		catch(const Connection::ConnectionClosedException& e)
		{
		}
	};

	mTimer = lib::Timer::instance().registerCallback(timer, mTimeout / 2, mTimeout / 2);

	mReaderThread.construct([this](void) {
		try
		{
			while(mRunning.load())
			{
				auto b = mClient->read();

				mLastRead = lib::Timer::instance().elapsed();

				if(!b.empty() && b.front() == static_cast<byte_t>(Packet::DATA))
				{
					b.erase(b.begin());

					mReadBuf.insert(b);
				}
			}
		}
		catch(const Connection::ConnectionClosedException& e)
		{
		}
		MXT_CATCH_STRAY
	});
}

Watchdog::~Watchdog(void)
{
	mRunning = false;

	lib::Timer::instance().unregisterCallback(mTimer);

	mClient.reset();

	mReaderThread.join();
}

void Watchdog::sendPacket(const Client::buffer_t& b)
{
	mClient->write(b);

	mLastWrite = lib::Timer::instance().elapsed();
}

}}
