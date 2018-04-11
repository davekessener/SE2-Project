#ifndef ESEP_HAL_PHYSICAL_H
#define ESEP_HAL_PHYSICAL_H

#include "lib/utils.h"
#include "hal/buffer.h"
#include "hal/gpio.h"

#define MXT_NGPIOS 3

namespace esep
{
	namespace hal
	{
		class Physical : public Buffer
		{
			public:
			using HAL::Field;
			using HAL::bitmask_t;

			public:
				Physical( );
				~Physical( );
				void out(Field, uint32_t);
				void set(Field, bitmask_t);
				void reset(Field, bitmask_t);
				void subscribeEvent(Event, callback_t);
			private:
				GPIO *mGPIOs[MXT_NGPIOS];
		};
	}
}

#endif

