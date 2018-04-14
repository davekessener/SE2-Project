#ifndef ESEP_LIB_WATCHDOG_H
#define ESEP_LIB_WATCHDOG_H

#include <stdexcept>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <memory>

#include "lib/utils.h"
#include "lib/member_wrapper.h"
#include "lib/timer.h"
#include "lib/logger.h"

namespace esep
{
	namespace lib
	{
		template
		<
			typename C,
			typename B = typename C::value_type,
			typename R = MemberWrapper<B, C>,
			typename W = MemberWrapper<void, C, const B&>
		>
		class Watchdog
		{
			public:
			typedef B value_type;
			MXT_DEFINE_E(TimeoutException);
			MXT_DEFINE_E(ClosingException);

			private:
			enum class Packet : byte_t
			{
				DATA = 0,
				WATCHDOG = 0xFF
			};

			typedef std::unique_lock<std::mutex> lock_t;
			typedef std::unique_ptr<C> client_ptr;

			struct component_t
			{
				std::thread thread;
				std::deque<value_type> buffer;
				std::mutex mutex;
				std::condition_variable cond;
				uint64_t time;
				std::atomic<bool> running;

				void checkTime(uint t)
				{
					if(Timer::instance().elapsed() - time > t)
						MXT_THROW_EX(TimeoutException);
				}

				void insert(const value_type& b)
				{
					{
						lock_t lock(mutex);

						buffer.push_back(b);
					}

					cond.notify_all();
				}

				value_type remove(uint timeout)
				{
					lock_t lock(mutex);

					checkTime(timeout);

					while(buffer.empty())
					{
						cond.wait(lock);

						if(!running.load())
							MXT_THROW_EX(ClosingException);

						checkTime(timeout);
					}

					value_type v(buffer.front());

					buffer.pop_front();

					return v;
				}
			};

			public:
				Watchdog(client_ptr, uint, R = &C::read, W = &C::write);
				~Watchdog( );
				value_type read( )
					{ return mReader.remove(mTimer); }
				void write(const value_type& b)
					{ value_type o(b); o.insert(o.begin(), static_cast<byte_t>(Packet::DATA)); mWriter.insert(o); }
			private:
				client_ptr mClient;
				R mRead;
				W mWrite;
				const uint mTimer;
				component_t mReader, mWriter;
				Timer::Class::id_t mTimerID;
		};

		template<typename C, typename B, typename R, typename W>
		Watchdog<C, B, R, W>::Watchdog(client_ptr c, uint t, R r, W w)
			: mClient(std::move(c))
			, mRead(r)
			, mWrite(w)
			, mTimer(t)
		{
			mReader.time = mWriter.time = Timer::instance().elapsed();
			mReader.running = mWriter.running = true;

			auto timer = [this](void) -> bool {
				if(Timer::instance().elapsed() - mWriter.time > mTimer / 2)
				{
					value_type b;

					b.insert(b.begin(), static_cast<byte_t>(Packet::WATCHDOG));

					mWriter.insert(b);
				}

				return mWriter.running.load();
			};

			mTimerID = Timer::instance().registerCallback(timer, mTimer / 2, mTimer / 2);

			mReader.thread = std::thread([this](void) {
				try
				{
					while(mReader.running.load())
					{
						value_type p = mRead(*mClient);

						if(p.empty()) continue;

						mReader.time = Timer::instance().elapsed();

						byte_t id = p.front();

						p.erase(p.begin());

						if(id == static_cast<byte_t>(Packet::DATA))
						{
							mReader.insert(p);
						}
					}
				}
				catch(const ClosingException& e)
				{
				}
				catch(const typename C::ClosingException& e)
				{
				}
				catch(const std::exception& e)
				{
					MXT_LOG(lib::stringify("Caught a stray exception: ", e.what()));
				}
				catch(const std::string& e)
				{
					MXT_LOG(lib::stringify("Caught a stray string: ", e));
				}
			});

			mWriter.thread = std::thread([this](void) {
				try
				{
					while(mWriter.running.load())
					{
						mWrite(*mClient, mWriter.remove(mTimer));

						mWriter.time = Timer::instance().elapsed();
					}
				}
				catch(const ClosingException& e)
				{
				}
				catch(const std::exception& e)
				{
					MXT_LOG(lib::stringify("Caught a stray exception: ", e.what()));
				}
				catch(const std::string& e)
				{
					MXT_LOG(lib::stringify("Caught a stray string: ", e));
				}
			});
		}

		template<typename C, typename B, typename R, typename W>
		Watchdog<C, B, R, W>::~Watchdog(void)
		{
			Timer::instance().unregisterCallback(mTimerID);

			mReader.running = mWriter.running = false;

			mWriter.cond.notify_all();
			mReader.cond.notify_all();

			mClient.reset();

			mWriter.thread.join();
			mReader.thread.join();
		}
	}
}

#endif
