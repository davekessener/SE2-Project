#ifndef ESEP_LIB_SYNC_MONITOR_H
#define ESEP_LIB_SYNC_MONITOR_H

#include "lib/thread.h"

#include "lib/function.h"

namespace esep
{
	namespace sync
	{
		/**
		 * Synchronizes access to an object.
		 *
		 * The held object can be accessed by passing a functor
		 * to the monitors operator() that accepts a reference
		 * to the held object as parameter.
		 */
		template<typename T>
		class Monitor
		{
			typedef std::unique_lock<std::mutex> lock_t;

			public:
				template<typename TT>
					explicit Monitor(TT&& o) : mObject(o) { }
				Monitor( ) { }
				template<typename F>
					typename lib::FunctionTraits<F>::return_type operator()(F&&);
			private:
				T mObject;
				std::mutex mMutex;
		};

		template<typename T>
		template<typename F>
		typename lib::FunctionTraits<F>::return_type Monitor<T>::operator()(F&& f)
		{
			lock_t lock(mMutex);

			return f(mObject);
		}
	}
}

#endif
