#include "lib/timer.h"

#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000l

#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000l

namespace esep { namespace lib { namespace timer {

Impl::TimerManager::TimerManager(TimerManager&& tm)
	: mID(INVALID_TIMER_ID)
{
	swap(tm);
}

Impl::TimerManager::~TimerManager(void)
{
	lib::Timer::instance().unregisterCallback(*this);
}

Impl::TimerManager& Impl::TimerManager::operator=(TimerManager&& tm)
{
	TimerManager t(INVALID_TIMER_ID);

	tm.swap(t);
	t.swap(*this);

	return *this;
}

void Impl::TimerManager::swap(TimerManager& tm) noexcept
{
	std::swap(mID, tm.mID);
}

// # --------------------------------------------------------------------------

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
					if(mUpdating.load())
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

Impl::TimerManager Impl::registerCallback(callback_t f, uint o, uint p)
{
	lock_t lock(mMutex);

	id_t id = mNextID++;

	mTimers.emplace(std::make_pair(id, Timer(id, f, o, p)));

	return TimerManager(id);
}

void Impl::unregisterCallback(const TimerManager& tm)
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

void Impl::update(void)
{
	mUpdating = true;

	auto timer_copy(mTimers);

	std::vector<id_t> timer_to_delete;

	for(auto& i : timer_copy)
	{
		auto& t(i.second);

		if(!t.next--)
		{
			bool should_exec = false;

			{
				lock_t lock(mMutex);

				should_exec = (mTimers.count(t.id) != 0);
			}

			if(should_exec)
			{
				bool should_delete = true;

				try
				{
					t.f();

					should_delete = false;
				}
				catch(const std::exception& e)
				{
					MXT_LOG(stringify("Caught an exception in timer: ", e.what()));
				}
				catch(const std::string& e)
				{
					MXT_LOG(stringify("Caught a string in timer: ", e));
				}
				catch(...)
				{
					MXT_LOG(stringify("Caught an unknown exception in timer!"));
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
