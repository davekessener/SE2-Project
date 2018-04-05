#ifndef ESEP_SYSTEM_H
#define ESEP_SYSTEM_H

#include "lib/utils.h"
#include "lib/singleton.h"

#include "hal/hal.h"

namespace esep
{
	namespace system
	{
		class Impl
		{
			public:
				Impl( );
				~Impl( );
				void run( );
				uint64_t elapsed( );
				void sleep(uint);
				hal::HAL& hal( ) { return *mHAL; }
			private:
				hal::HAL *mHAL;
		};
	}

	typedef lib::SingletonHolder<system::Impl> System;
}

#endif
