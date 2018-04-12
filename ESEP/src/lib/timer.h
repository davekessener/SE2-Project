#ifndef ESEP_LIB_TIMER_H
#define ESEP_LIB_TIMER_H

#include <functional>

#include "lib/utils.h"
#include "lib/singleton.h"

namespace esep
{
	namespace lib
	{
		namespace timer
		{
			class Impl
			{
				public:
				typedef std::function<bool(void)> callback_t;

				public:
					Impl( );
					void registerCallback(callback_t, uint, uint = 0);
					uint64_t elapsed( );
					void sleep(uint);
					void reset( );
				private:
					std::chrono::time_point<std::chrono::system_clock> mSystemStart;
			};
		}

		typedef SingletonHolder<timer::Impl> Timer;
	}
}

#endif
