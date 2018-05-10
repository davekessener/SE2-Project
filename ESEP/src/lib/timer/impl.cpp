#include <pthread.h>

#include "lib/timer/impl.h"
#include "lib/timer/holder.h"
#include "lib/timer/async.h"

#include "lib/utils.h"
#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000l
#define MXT_TOLERANCE 15 // max of 10ms delay is acceptable

namespace esep { namespace timer {

namespace
{
	constexpr uint diff(const uint a, const uint b) { return a > b ? a - b : b - a; }

	enum class Code : int8_t
	{
		SHUTDOWN,
		EXPIRED
	};
}

Impl::Impl(void)
{
	reset();

	mRunning = true;
	mNextID = INVALID_TIMER_ID + 1;
	mCounter = 0;

	mTimerThread.construct([this](void) {
		try
		{
			qnx::Channel channel;
			bool overshot = false;

			mConnection = channel.connect();

			channel.registerTimerListener(mConnection, static_cast<int8_t>(Code::EXPIRED), MXT_1MS_IN_NS);

			pthread_setschedprio(pthread_self(), 250);

			while(mRunning.load())
			{
				qnx::pulse_t p = channel.receivePulse();

				switch(p.code)
				{
				case static_cast<int8_t>(Code::SHUTDOWN):
					mRunning = false;
					break;

				case static_cast<int8_t>(Code::EXPIRED):
					update();
					break;

				default:
					MXT_LOG(lib::stringify("Received unknown pulse msg {", lib::hex<8>(p.code), ", ", lib::hex<32>(p.value), " (", p.value, ")}"));
				}

				if(mRunning.load())
				{
					++mCounter;

					auto d = diff(mCounter, elapsed());

					if(!overshot && d > MXT_TOLERANCE)
					{
						MXT_LOG(lib::stringify("Significantly overshot timer by ", d, "ms!"));

						overshot = true;
					}
					else if(overshot && d <= MXT_TOLERANCE)
					{
						overshot = false;
					}
				}
			}
		}
		MXT_CATCH_ALL_STRAY
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

	MXT_LOG("Timer was shut down.");
}

Manager Impl::registerCallback(callback_fn f, uint r, uint p)
{
	return addTimer(Timer_ptr(new Holder(nextID(), std::move(f), r, p)));
}

Manager Impl::registerAsync(callback_fn f, uint r, uint p)
{
	return addTimer(Timer_ptr(new Async(nextID(), std::move(f), r, p)));
}

void Impl::unregisterCallback(const Manager& tm)
{
	lock_t lock;

	if(!mTimerThread.active())
	{
		lock = lock_t(mMutex);
	}

	auto i = mTimers.find(tm.mID);

	if(i != mTimers.end())
	{
		i->second->shutdown();
	}
}

Manager Impl::addTimer(Timer_ptr p)
{
	auto id = p->ID();

	if(mTimerThread.active())
	{
		mQueue.emplace_front(std::move(p));
	}
	else
	{
		MXT_SYNCHRONIZE;

		mTimers.emplace(std::make_pair(id, std::move(p)));
	}

	return Manager(id);
}

uint64_t Impl::elapsed(void)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mSystemStart).count();
}

void Impl::reset(void)
{
	mSystemStart = std::chrono::system_clock::now();
	mCounter = 0;
}

id_t Impl::nextID(void)
{
	return mNextID++;
}

void Impl::update(void)
{
	MXT_SYNCHRONIZE;

	for(auto i1 = mTimers.begin(), i2 = mTimers.end() ; i1 != i2 ;)
	{
		auto& t(*i1->second);

		if(t.done())
		{
			i1 = mTimers.erase(i1);
		}
		else
		{
			t.tick();

			++i1;
		}
	}

	for(auto& p : mQueue)
	{
		auto id = p->ID();
		mTimers.emplace(id, std::move(p)); // evaluation order!
	}

	mQueue.clear();
}

}}
