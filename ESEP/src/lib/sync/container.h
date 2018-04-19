#ifndef ESEP_LIB_SYNC_CONTAINER_H
#define ESEP_LIB_SYNC_CONTAINER_H

#include <deque>
#include "lib/thread.h"
#include <atomic>
#include "lib/thread.h"

#include "lib/member_wrapper.h"
#include "lib/sync/auto_counter.h"

#include "lib/logger.h"

namespace esep
{
	namespace sync
	{
		template
		<
			typename T,
			typename C = std::deque<T>,
			typename Access = lib::ConstMemberWrapper<const T&, C>,
			typename Insert = lib::MemberWrapper<void, C, const T&>,
			typename Remove = lib::MemberWrapper<void, C>
		>
		class Container
		{
			typedef AutoCounter<std::atomic<uint>> counter_t;

			public:
			typedef T value_type;
			typedef C container_type;
			typedef Access access_fn;
			typedef Insert insert_fn;
			typedef Remove remove_fn;
			typedef std::unique_lock<std::mutex> lock_t;

			MXT_DEFINE_E(InterruptedException);

			public:
				Container(
					access_fn a = access_fn(&container_type::front),
					insert_fn i = insert_fn(&container_type::push_back),
					remove_fn r = remove_fn(&container_type::pop_front))
						: mAccess(a), mInsert(i), mRemove(r), mSize(0), mInterrupted(false), mReaders(0) { }
				~Container( ) { mInterrupted = true; while(mReaders.load()) mCond.notify_all(); }
				void insert(const value_type&);
				value_type remove( );
				size_t size( ) const { return mSize; }
				bool empty( ) const { return !mSize; }
				void clear( ) { lock_t lock(mMutex); while(mSize) { --mSize; mRemove(mContainer); } }
			private:
				container_type mContainer;
				access_fn mAccess;
				insert_fn mInsert;
				remove_fn mRemove;
				std::mutex mMutex;
				std::condition_variable mCond;
				size_t mSize;
				std::atomic<bool> mInterrupted;
				std::atomic<uint> mReaders;
		};

		template<typename T, typename C, typename A, typename I, typename R>
		void Container<T, C, A, I, R>::insert(const value_type& o)
		{
			{
				lock_t lock(mMutex);

				if(mInterrupted.load())
					MXT_THROW_EX(InterruptedException);

				mInsert(mContainer, o);
				++mSize;
			}

			mCond.notify_all();
		}

		template<typename T, typename C, typename A, typename I, typename R>
		T Container<T, C, A, I, R>::remove(void)
		{
			counter_t count(mReaders);

			lock_t lock(mMutex);

			while(mContainer.empty())
			{
				mCond.wait(lock);

				if(mInterrupted.load())
					MXT_THROW_EX(InterruptedException);
			}

			value_type o(mAccess(mContainer));

			mRemove(mContainer);
			--mSize;

			return o;
		}
	}
}

#endif
