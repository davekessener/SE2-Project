#include <chrono>
#include <thread>

#include "lib/timer.h"

#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000l

namespace esep { namespace lib { namespace timer {

Impl::Impl(void)
{
	reset();

	mRunning = true;
	mUpdating = false;

	mTimerThread = std::thread([this](void) {
		try
		{
			qnx::Channel channel;

			mConnection = channel.connect();

			channel.registerTimerListener(mConnection, static_cast<int8_t>(Code::EXPIRED), MXT_1MS_IN_NS);

			while(mRunning)
			{
				qnx::pulse_t p = channel.receivePulse();

				switch(p.code)
				{
				case static_cast<int8_t>(Code::SHUTDOWN):
					break;
				case static_cast<int8_t>(Code::EXPIRED):
					if(mUpdating)
					{
						throw TimerOverflowException();
					}
					update();
					break;
				default:
					MXT_LOG(lib::stringify("Received unknown pulse msg {", (int)p.code, ", ", lib::hex<32>(p.value), " (", p.value, ")}"));
				}
			}
		}
		catch(const std::string& e)
		{
			MXT_LOG(lib::stringify("Caught a stray string: ", e));
		}
		catch(const std::exception& e)
		{
			MXT_LOG(lib::stringify("Caught a stray exeception: ", e.what()));
		}
	});
}

Impl::~Impl(void)
{
	mRunning = false;
	try
	{
		mConnection.sendPulse(static_cast<int8_t>(Code::SHUTDOWN));
	}
	catch(...)
	{
		MXT_LOG("Failed to send shutdown signal; may hang!");
	};
	mTimerThread.join();
}

void Impl::reset(void)
{
	mSystemStart = std::chrono::system_clock::now();
}

void Impl::registerCallback(callback_t f, uint o, uint p)
{
	lock_t lock(mMutex);

	mTimers.push_back(Timer(f, o, p));
}

void Impl::sleep(uint ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint64_t Impl::elapsed(void)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mSystemStart).count();
}

void Impl::update(void)
{
	lock_t lock(mMutex);

	mUpdating = true;

	for(auto i1 = std::begin(mTimers), i2 = std::end(mTimers) ; i1 != i2 ; )
	{
		auto& t(*i1);

		if(!t.next--)
		{
			if(t.f() && t.period)
			{
				t.next = t.period;
			}
			else
			{
				i1 = mTimers.erase(i1);
			}
		}
		else
		{
			++i1;
		}
	}

	mUpdating = false;
}

}}}
