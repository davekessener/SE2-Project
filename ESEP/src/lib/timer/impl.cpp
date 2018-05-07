#include <pthread.h>

#include "lib/timer/impl.h"

#include "lib/utils.h"
#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000l
#define MXT_TOLERANCE 30 // max of 10ms delay is acceptable

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
						MXT_LOG("Timer recovered.");

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
}

Manager Impl::registerCallback(callback_t f, uint o, uint p)
{
	lock_t lock(mMutex);

	id_t id = nextID();

	mTimers.insert(id, Timer(id, f, o, p));

	return Manager(id);
}

Manager Impl::registerAsync(callback_t f, uint o, uint p)
{
	return registerCallback(std::move(f), o, p);
//	lock_t lock(mMutex);
//
//	id_t id = nextID();
//
//	mAsyncs.insert(id, Async_ptr(new Async(id, f, o, p)));
//
//	return Manager(id);
}

void Impl::unregisterCallback(const Manager& tm)
{
	lock_t lock(mMutex);

	mAsyncs.with(tm.mID, [](Async_ptr& p) { p->shutdown(); });
	mTimers.remove(tm.mID);
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
	{
		lock_t lock(mMutex);

		mTimers.update();
		mAsyncs.update();
	}

	for(auto i1 = mTimers.begin(), i2 = mTimers.end() ; i1 != i2 ;)
	{
		bool should_erase = false;
		auto& t(i1->second);

		if(!t.next--)
		{
			should_erase = true;

			try
			{
				t.f();

				should_erase = !(t.next = t.period);
			}
			MXT_CATCH_ALL_STRAY
		}

		if(should_erase)
		{
			i1 = mTimers.erase(i1);
		}
		else
		{
			++i1;
		}
	}

	for(auto i1 = mAsyncs.begin(), i2 = mAsyncs.end() ; i1 != i2 ;)
	{
		auto& t(*(i1->second));

		t.tick();

		if(t.done())
		{
			i1 = mAsyncs.erase(i1);
		}
		else
		{
			++i1;
		}
	}
}

}}
