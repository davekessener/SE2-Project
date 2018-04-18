#include "serial/watchdog.h"

#include "lib/logger.h"

namespace esep { namespace serial {

Watchdog::Watchdog(client_ptr c, uint t)
	: mClient(std::move(c))
//	, mTimeout(t)
{
	mLastRead = mLastWrite = lib::Timer::instance().elapsed();
	mTimedOut = false;
	mRunning = true;

	auto timer = [this](void) -> bool {
		auto e = lib::Timer::instance().elapsed();

		if(e - mLastWrite > mTimeout / 2)
		{
			Client::buffer_t b;

			b.push_back(static_cast<byte_t>(Packet::WATCHDOG));

			sendPacket(b);
		}

		return mRunning.load();
	};

//	mTimerID = lib::Timer::instance().registerCallback(timer, mTimeout / 2, mTimeout / 2);

//	mReaderThread = std::thread([this](void) {
//		try
//		{
//			while(mRunning.load())
//			{
//				auto b = mClient->read();
//
//				mLastRead = lib::Timer::instance().elapsed();
//
//				if(!b.empty() && b.front() == static_cast<byte_t>(Packet::DATA))
//				{
//					b.erase(b.begin());
//
//					mReadBuf.insert(b);
//				}
//			}
//		}
//		MXT_CATCH_STRAY
//	});
}

Watchdog::~Watchdog(void)
{
//	mRunning.store(false);

//	lib::Timer::instance().unregisterCallback(mTimerID);

//	mClient.reset();

//	mReaderThread.join();
}

void Watchdog::sendPacket(const Client::buffer_t& b)
{
	mClient->write(b);

	mLastWrite = lib::Timer::instance().elapsed();
}

}}
