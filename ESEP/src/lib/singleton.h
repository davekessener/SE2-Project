#ifndef TML_SINGLETON_H
#define TML_SINGLETON_H

#include <functional>
#include <deque>

#include "lib/tml.h"

namespace esep
{
	namespace lib
	{
		class ExitManager
		{
			typedef std::function<void(void)> cb_fn;
			typedef std::pair<int, cb_fn> entry_t;
			typedef std::deque<entry_t> container_t;
			typedef std::unique_lock<std::mutex> lock_t;

			public:
				static void atExit(cb_fn f, int p = 0)
				{
					static std::mutex mtx;
					lock_t lock(mtx);
					entry_t e(p, std::move(f));

					for(auto i1 = callbacks().begin(), i2 = callbacks().end() ; i1 != i2 ; ++i1)
					{
						if(i1->first <= p)
						{
							callbacks().insert(i1, std::move(e));

							return;
						}
					}

					callbacks().emplace_back(std::move(e));
				}

				static void execute( )
				{
					while(!callbacks().empty())
					{
						entry_t e(callbacks().front());

						callbacks().pop_front();

						e.second();
					}
				}

			private:
				static container_t& callbacks( ) { static container_t o; return o; }

				ExitManager( ) = delete;
				ExitManager(const ExitManager&) = delete;
				~ExitManager( ) = delete;
				ExitManager& operator=(const ExitManager&) = delete;
		};

		template
		<
			typename T,
			int Priority = -1,
			typename CreationPolicy = tml::GenScatterHierarchy<tml::policy::DynamicCreation<T>, tml::policy::OneShotLifetime>,
			typename ThreadingPolicy = tml::policy::SingleThreaded
		>
		class Singleton
		{
			typedef Singleton<T, Priority, CreationPolicy, ThreadingPolicy> Self;
			typedef typename ThreadingPolicy::Mutex Mutex;
			typedef typename ThreadingPolicy::Lock Lock;

			public:
			typedef T Class;

			struct InvalidDeletionException : std::exception { };

			public:
				static Class& instance( )
				{
					static bool was_created = false;

					if(!sInstance)
					{
						create(was_created);
					}

					return *sInstance;
				}

			private:
				static void create(bool& was_created)
				{
					Lock lock(sMutex);

					if(!sInstance)
					{
						if(was_created)
						{
							CreationPolicy::onDeadReference();
						}

						was_created = true;
						ExitManager::atExit([](void) {
							Self::destroy();
						}, Priority);

						sInstance = CreationPolicy::create();
					}
				}

				static void destroy( )
				{
					Lock lock(sMutex);

					CreationPolicy::destroy(sInstance);
					sInstance = nullptr;
				}

			private:
				static Class *sInstance;
				static Mutex sMutex;

				Singleton( ) = delete;
				Singleton(const Self&) = delete;
				~Singleton( ) = delete;
				Self& operator=(const Self&) = delete;
		};

		template<typename T, int P, typename CP, typename TP>
		T *Singleton<T, P, CP, TP>::sInstance = nullptr;

		template<typename T, int P, typename CP, typename TP>
		typename TP::Mutex Singleton<T, P, CP, TP>::sMutex;
	}
}

#endif
