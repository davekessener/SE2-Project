#include "serial/watchdog.h"

#include "lib/logger.h"

namespace esep { namespace serial {

Watchdog::Watchdog(client_ptr c, uint t)
	: mClient(std::move(c))
	, mTimeout(t)
{
	mLastRead = mLastWrite = 0;
	mTimedOut = false;
	mRunning = true;
	mIsActive = false;

	auto cb = [this](void) {
		if(mRunning.load()) try
		{
			auto e = lib::Timer::instance().elapsed();

			if(e - mLastWrite.load() > mTimeout / 2)
			{
				sendWatchdog();
			}

			if(e - mLastRead.load() > mTimeout)
			{
				mTimedOut = true;
				mReadBuf.interrupt();
			}
		}
		catch(const Connection::ConnectionClosedException& e)
		{
			mTimedOut = true;
			mReadBuf.interrupt();
		}
	};

	mReaderThread.construct([this, cb](void) {
		try
		{
			sendWatchdog();

			while(mRunning.load())
			{
				auto b = mClient->read();

				mLastRead = lib::Timer::instance().elapsed();

				if(!mIsActive.load())
				{
					mIsActive = true;

					mTimer = lib::Timer::instance().registerCallback(cb, 1, mTimeout / 4);
				}

				if(!b.empty() && b.front() == static_cast<byte_t>(Packet::DATA))
				{
					b.erase(b.begin());

					mReadBuf.insert(b);
				}
			}
		}
		catch(const container_t::InterruptedException& e)
		{
			mTimedOut = true;
		}
		catch(const Connection::ConnectionClosedException& e)
		{
			mTimedOut = true;
		}
		MXT_CATCH_STRAY
	});

	while(mLastWrite.load() == 0)
	{
		lib::Timer::instance().sleep(1);
	}
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
	mLastWrite = lib::Timer::instance().elapsed();

	mClient->write(b);
}

void Watchdog::write(const Client::buffer_t& o)
{
	if(mTimedOut.load())
	{
		throw Connection::ConnectionClosedException();
	}

	auto b(o);

	b.insert(b.begin(), static_cast<byte_t>(Packet::DATA));

	sendPacket(b);
}

Client::buffer_t Watchdog::read(void)
{
	if(mTimedOut.load())
	{
		throw Connection::ConnectionClosedException();
	}

	try
	{
		return mReadBuf.remove();
	}
	catch(const decltype(mReadBuf)::InterruptedException& e)
	{
		throw Connection::ConnectionClosedException();
	}
}

}}
