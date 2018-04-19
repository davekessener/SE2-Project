#ifndef ESEP_LIB_SYNC_MONITOR_H
#define ESEP_LIB_SYNC_MONITOR_H

#include "lib/thread.h"

#include "lib/function.h"

namespace esep
{
	namespace sync
	{
		template<typename T>
		class Monitor
		{
			typedef std::unique_lock<std::mutex> lock_t;

			public:
				template<typename TT>
					explicit Monitor(TT&& o) : mObject(o) { }
				Monitor( ) { }
				template<typename F>
					auto operator()(F&&) -> typename lib::FunctionTraits<F>::return_type;
			private:
				T mObject;
				std::mutex mMutex;
		};

		template<typename T>
		template<typename F>
		auto Monitor<T>::operator()(F&& f) -> typename lib::FunctionTraits<F>::return_type
		{
			lock_t lock(mMutex);

			return f(mObject);
		}
	}
}

#endif
