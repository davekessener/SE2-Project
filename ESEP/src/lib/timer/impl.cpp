#include <pthread.h>

#include "lib/timer/impl.h"

#include "lib/utils.h"
#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000l
#define MXT_TOLERANCE 10 // max of 10ms delay is acceptable

namespace esep { namespace timer {

namespace
{
	constexpr uint diff(const uint a, const uint b) { return a > b ? a - b : b - a; }
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

	id_t id = mNextID++;

	mTimers.emplace(std::make_pair(id, Timer(id, f, o, p)));

	return Manager(id);
}

void Impl::unregisterCallback(const Manager& tm)
{
	lock_t lock(mMutex);

	auto i = mTimers.find(tm.mID);

	if(i != mTimers.end())
	{
		mTimers.erase(i);
	}
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

void Impl::update(void)
{
	decltype(mTimers) timer_copy;
	std::vector<id_t> timer_to_delete;

	{
		lock_t lock(mMutex);

		timer_copy = mTimers;
	}

	for(auto& i : timer_copy)
	{
		auto& t(i.second);

		if(!t.next--)
		{
			bool should_delete = true;

			try
			{
				t.f();

				should_delete = false;
			}
			catch(const std::exception& e)
			{
				MXT_LOG(lib::stringify("Caught an exception in timer: ", e.what()));
			}
			catch(const std::string& e)
			{
				MXT_LOG(lib::stringify("Caught a string in timer: ", e));
			}
			catch(...)
			{
				MXT_LOG(lib::stringify("Caught an unknown exception in timer!"));
			}

			if(should_delete || !t.period)
			{
				timer_to_delete.push_back(t.id);
			}
			else
			{
				t.next = t.period;
			}
		}
	}

	{
		lock_t lock(mMutex);

		for(const auto& id : timer_to_delete)
		{
			auto i = mTimers.find(id);

			if(i != mTimers.end())
			{
				mTimers.erase(i);
			}
		}

		for(auto& p : timer_copy)
		{
			auto& t(p.second);
			auto i = mTimers.find(t.id);

			if(i != mTimers.end())
			{
				i->second = t;
			}
		}
	}
}

}}
