#include "serial/watchdog.h"

#include "lib/logger.h"

namespace esep { namespace serial {

Watchdog::Watchdog(client_ptr c, uint t)
	: mClient(std::move(c))
	, mStartTime(std::chrono::system_clock::now())
	, mTimeout(t)
{
	mLastRead = mLastWrite = 0;
	mTimedOut = false;
	mRunning = true;
	mIsActive = false;

	auto cb = [this](void) {
		if(mRunning.load()) try
		{
			auto e = elapsed();
			auto w = mLastWrite.load();
			auto r = mLastRead.load();

			if(e > w && e - w > mTimeout / 2)
			{
				sendWatchdog();
			}

			if(e > r && e - r > mTimeout)
			{
				MXT_LOG_ERROR("Watchdog triggered time-out (", mTimeout, "ms): {e=",e, ", last_read=", r, "}");

				mTimedOut = true;
				mReadBuf.interrupt();
				mTimer.reset();
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

				mLastRead = elapsed();

				if(!mIsActive.load())
				{
					mIsActive = true;

					mTimer = lib::Timer::instance().registerAsync(cb, 1, mTimeout / 4);
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

	uint to = 100;
	while(mLastWrite.load() == 0 && --to)
	{
		lib::Timer::instance().sleep(1);
	}

	if(!to)
	{
		MXT_THROW_EX(CantSendException);
	}
}

Watchdog::~Watchdog(void)
{
	mRunning = false;
	mTimer.reset();
	mClient.reset();
	mReaderThread.join();
}

void Watchdog::sendPacket(const Client::buffer_t& b)
{
	mLastWrite = elapsed();

	mClient->write(b);
}

void Watchdog::write(const Client::buffer_t& o)
{
	if(mTimedOut.load())
	{
		MXT_THROW_EX(Connection::ConnectionClosedException);
	}

	auto b(o);

	b.insert(b.begin(), static_cast<byte_t>(Packet::DATA));

	sendPacket(b);
}

Client::buffer_t Watchdog::read(void)
{
	if(mTimedOut.load())
	{
		MXT_THROW_EX(Connection::ConnectionClosedException);
	}

	try
	{
		return mReadBuf.remove();
	}
	catch(const decltype(mReadBuf)::InterruptedException& e)
	{
		MXT_THROW_EX(Connection::ConnectionClosedException);
	}
}

}}
