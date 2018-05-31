#ifndef ESEP_LIB_CONSOLE_H
#define ESEP_LIB_CONSOLE_H

#include <string>
#include <mutex>
#include <functional>

#include "lib/utils.h"
#include "lib/thread.h"
#include "lib/singleton.h"

#include "lib/sync/monitor.h"

namespace esep
{
	namespace lib
	{
		namespace console
		{
			class Impl
			{
				typedef std::unique_lock<std::mutex> lock_t;
				typedef std::function<void(const std::string&)> callback_fn;

				public:
					template<typename ... T>
						void println(T&& ... o) { doPrint(stringify(std::forward<T>(o)..., "\n")); }
					template<typename ... T>
						void print(T&& ... o) { doPrint(stringify(std::forward<T>(o)...)); }
					void setCallback(callback_fn);

				private:
					Impl( );
					~Impl( );
					Impl(const Impl&) = delete;
					Impl& operator=(const Impl&) = delete;

					void doPrint(const std::string&);

				private:
					std::mutex mMutex;
					sync::Monitor<callback_fn> mCallback;
					Thread mReader;

					friend class tml::policy::DynamicCreation<Impl>;
			};
		}

		typedef Singleton<console::Impl> Console;
	}
}

#endif
