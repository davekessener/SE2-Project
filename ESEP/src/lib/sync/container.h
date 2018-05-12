#ifndef ESEP_LIB_SYNC_CONTAINER_H
#define ESEP_LIB_SYNC_CONTAINER_H

#include <deque>
#include <functional>
#include <atomic>

#include "lib/thread.h"

#include "lib/member_wrapper.h"
#include "lib/sync/auto_counter.h"

#include "lib/logger.h"

namespace esep
{
	namespace sync
	{
		/**
		 * Thread-safe container for objects of type T.
		 *
		 * Can be instantiated with a custom:
		 * - container type C (defaults to std::deque<T>)
		 * - Insert functor (defaults to wrapper of C::push_back)
		 * - Remove functor (defaults to wrapper of C::front + C::pop_front)
		 *
		 * The remove() method blocks and suspends the calling thread
		 * until an object to be removed is inserted into the Container.
		 *
		 * While a thread is blocked in remove() another thread may
		 * interrupt it by either calling the interrupt() method or
		 * destroying the Container object. In that case the receive()
		 * call is terminated by throwing an InterruptedException.
		 */
		template
		<
			typename T,
			typename C = std::deque<T>
		>
		class Container
		{
			typedef AutoCounter<std::atomic<uint>> counter_t;

			public:
			typedef T value_type;
			typedef C container_type;
			typedef std::function<void(container_type&, const value_type&)> insert_fn;
			typedef std::function<value_type(container_type&)> remove_fn;
			typedef std::unique_lock<std::mutex> lock_t;

			MXT_DEFINE_E(InterruptedException);

			public:
				Container(
					insert_fn i = [](container_type& c, const value_type& v) { c.push_back(v); },
					remove_fn r = [](container_type& c) { value_type v(c.front()); c.pop_front(); return v; })
						: mInsert(i), mRemove(r), mSize(0), mInterrupted(false), mReaders(0) { }
				~Container( ) { interrupt(true); }
				void insert(const value_type&);
				value_type remove( );
				size_t size( ) const { return mSize; }
				bool empty( ) const { return !mSize; }
				void clear( ) { MXT_SYNCHRONIZE; while(mSize) { --mSize; mRemove(mContainer); } }
				void interrupt(bool final = false)
					{ mInterrupted = true; while(mReaders.load()) mCond.notify_all(); mInterrupted = final; }
			private:
				container_type mContainer;
				insert_fn mInsert;
				remove_fn mRemove;
				std::mutex mMutex;
				std::condition_variable mCond;
				size_t mSize;
				std::atomic<bool> mInterrupted;
				std::atomic<uint> mReaders;
		};

		template<typename T, typename C>
		void Container<T, C>::insert(const value_type& o)
		{
			{
				MXT_SYNCHRONIZE;

				if(mInterrupted.load())
					MXT_THROW_EX(InterruptedException);

				mInsert(mContainer, o);
				++mSize;
			}

			mCond.notify_all();
		}

		template<typename T, typename C>
		T Container<T, C>::remove(void)
		{
			counter_t count(mReaders);

			MXT_SYNCHRONIZE;

			while(mContainer.empty())
			{
				mCond.wait(lock);

				if(mInterrupted.load())
					MXT_THROW_EX(InterruptedException);
			}

			--mSize;

			return mRemove(mContainer);
		}
	}
}

#endif
