#ifndef ESEP_TML_POLICY_H
#define ESEP_TML_POLICY_H

#include <mutex>

#include "lib/tml/base.h"

namespace esep
{
	namespace tml
	{
		namespace policy
		{
			struct SingleThreaded
			{
				struct Mutex { };
				struct Lock { Lock(Mutex&) { } };
			};

			template
			<
				typename M = std::mutex,
				typename L = Fun2Type<std::unique_lock>
			>
			struct MultiThreaded
			{
				typedef M Mutex;
				typedef DoCall<L, M> Lock;
			};

// # ----------------------------------------------------------------------

			template<typename ThreadingPolicy>
			struct InstanceSynchronization
			{
				typedef typename ThreadingPolicy::Mutex Mutex;

				struct Lock : ThreadingPolicy::Lock
				{
					Lock(InstanceSynchronization<ThreadingPolicy>& o)
						: ThreadingPolicy::Lock(o.mMutex) { }
				};

				private:
					Mutex mMutex;
			};

			template
			<
				typename T,
				typename ThreadingPolicy
			>
			struct ClassSynchronization
			{
				typedef typename ThreadingPolicy::Mutex Mutex;

				struct Lock : ThreadingPolicy::Lock
				{
					Lock(ClassSynchronization<T, ThreadingPolicy>& o)
						: ThreadingPolicy::Lock(sMutex) { }
				};

				private:
					static Mutex sMutex;
			};

			template<typename T, typename TP>
			typename TP::Mutex ClassSynchronization<T, TP>::sMutex;

			template<typename ThreadingPolicy>
			struct SynchronizeClass
			{
				template<typename T>
				using Apply = Type2Type<ClassSynchronization<T, ThreadingPolicy>>;
			};

			template<typename C, typename TP>
			using ThreadingInheritor = DoIf<CanCall<TP, C>, Call<TP, C>, Type2Type<TP>>;

// # ----------------------------------------------------------------------

			template<typename T>
			struct DynamicCreation
			{
				static T *create( ) { return new T; }
				static void destroy(T *t) { delete t; }
			};

			template<typename T>
			struct StaticCreation
			{
				static T *create( ) { static T o; return o; }
				static void destroy(T *) { }
			};

// # ----------------------------------------------------------------------

			struct PhoenixLifetime { static void onDeadReference( ) { } };

			struct OneShotLifetime
			{
				struct RecreationException : std::exception { };

				static void onDeadReference( )
				{
					throw RecreationException();
				}
			};
		}
	}
}

#endif
