#include "lib/timer/async.h"

#include "lib/logger.h"

namespace esep { namespace timer {

namespace
{
	enum class Code : int8_t
	{
		EXECUTE,
		SHUTDOWN
	};
}

Async::Async(id_t id, callback_fn f, uint r, uint p)
	: Base(id, r, p)
	, mCallback(std::move(f))
{
	mRunning = true;
	mIsActive = true;

	mThread.construct([this](void) {
		qnx::Channel channel;

		try
		{
			mConnection = channel.connect();

			while(mRunning.load())
			{
				auto p = channel.receivePulse();

				switch(p.code)
				{
				case static_cast<int8_t>(Code::EXECUTE):
					if(mRunning.load())
					{
						mCallback();
					}
					break;

				case static_cast<int8_t>(Code::SHUTDOWN):
					mRunning = false;
					break;

				default:
					break;
				}
			}
		}
		MXT_CATCH_ALL_STRAY

		mIsActive = false;
	});
}

Async::~Async(void)
{
	if(!done())
	{
		MXT_LOG("Async timer did not properly shut down!");

		try
		{
			mConnection.sendPulse(Code::SHUTDOWN);
		}
		MXT_CATCH_ALL_STRAY
	}
}

void Async::tick(void)
{
	if(!mIsActive.load() || !mRunning.load() || !mConnection.isConnected())
		return;

	Base::tick();
}

void Async::shutdown(void)
{
	if(done()) return;

	mRunning = false;

	if(mConnection.isConnected())
	{
		mConnection.sendPulse(Code::SHUTDOWN);
	}

	Base::shutdown();
}

void Async::execute(void)
{
	mConnection.sendPulse(Code::EXECUTE);
}

}}
