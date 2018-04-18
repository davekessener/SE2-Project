#include "lib/timer.h"

#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000l

namespace esep { namespace lib { namespace timer {

Impl::Impl(void)
{
	reset();

	mRunning = true;
	mUpdating = false;
	mNextID = INVALID_TIMER_ID + 1;

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

Impl::id_t Impl::registerCallback(callback_t f, uint o, uint p)
{
	lock_t lock(mMutex);

	id_t id = mNextID++;

	mTimers.emplace(std::make_pair(id, Timer(id, f, o ? o : 1, p)));

	return id;
}

void Impl::unregisterCallback(id_t id)
{
	lock_t lock(mMutex);

	auto i = mTimers.find(id);

	if(i != mTimers.end())
	{
		mTimers.erase(i);
	}
}

uint64_t Impl::elapsed(void)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mSystemStart).count();
}

void Impl::update(void)
{
	auto timer_copy(mTimers);

	std::vector<id_t> timer_to_delete;

	mUpdating = true;

	for(auto& i : timer_copy)
	{
		auto& t(i.second);

		if(!t.next--)
		{
			if(t.f() && t.period)
			{
				t.next = t.period;
			}
			else
			{
				timer_to_delete.push_back(t.id);
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

	mUpdating = false;
}

}}}
