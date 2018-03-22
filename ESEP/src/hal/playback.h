#ifndef ESEP_HAL_PLAYBACK_H
#define ESEP_HAL_PLAYBACK_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class Playback : public HAL
		{
			using HAL::Port;

			public:
				uint8_t in8(Port);
				uint16_t in16( );
				void out8(Port, uint8_t);
			private:
		};
	}
}

#endif

