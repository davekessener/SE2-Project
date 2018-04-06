#ifndef ESEP_SYSTEM_H
#define ESEP_SYSTEM_H

#include "lib/utils.h"
#include "lib/singleton.h"

#include "hal/hal.h"
#include "hal/buffer.h"

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
				void updateHAL(hal::HAL::Field f, uint32_t v) { mHAL->update(f, v); }
			private:
				hal::Buffer *mHAL;
		};
	}

	typedef lib::SingletonHolder<system::Impl> System;
}

#endif
