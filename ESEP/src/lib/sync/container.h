#ifndef ESEP_LIB_SYNC_CONTAINER_H
#define ESEP_LIB_SYNC_CONTAINER_H

#include <deque>
#include <mutex>
#include <condition_variable>

#include "lib/member_wrapper.h"

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
			public:
			typedef T value_type;
			typedef C container_type;
			typedef Access access_fn;
			typedef Insert insert_fn;
			typedef Remove remove_fn;
			typedef std::unique_lock<std::mutex> lock_t;

			public:
				Container(
					access_fn a = access_fn(&container_type::front),
					insert_fn i = insert_fn(&container_type::push_back),
					remove_fn r = remove_fn(&container_type::pop_front))
				: mAccess(a), mInsert(i), mRemove(r) { }
				void insert(const value_type&);
				value_type remove( );
			private:
				container_type mContainer;
				access_fn mAccess;
				insert_fn mInsert;
				remove_fn mRemove;
				std::mutex mMutex;
				std::condition_variable mCond;
		};

		template<typename T, typename C, typename A, typename I, typename R>
		void Container<T, C, A, I, R>::insert(const value_type& o)
		{
			{
				lock_t lock(mMutex);

				mInsert(mContainer, o);
			}

			mCond.notify_all();
		}

		template<typename T, typename C, typename A, typename I, typename R>
		T Container<T, C, A, I, R>::remove(void)
		{
			lock_t lock(mMutex);

			while(mContainer.empty())
			{
				mCond.wait(lock, [this](void) { return !mContainer.empty(); });
			}

			value_type o(mAccess(mContainer));

			mRemove(mContainer);

			return o;
		}
	}
}

#endif
