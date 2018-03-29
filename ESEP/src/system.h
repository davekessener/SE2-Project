#ifndef ESEP_SYSTEM_H
#define ESEP_SYSTEM_H

#include "lib/utils.h"
#include "lib/singleton.h"

namespace esep
{
	namespace system
	{
		class Impl
		{
			public:
				uint64_t elapsed( );
			private:
		};
	}

	typedef lib::SingletonHolder<system::Impl> System;
}

#endif

