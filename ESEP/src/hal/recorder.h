#ifndef ESEP_HAL_RECORDER_H
#define ESEP_HAL_RECORDER_H

#include <iostream>

#include "hal/hal.h"
#include "lib/io.h"

namespace esep
{
	namespace hal
	{
		class Recorder : public HAL
		{
			using HAL::Port;

			public:
				Recorder(HAL *, std::ostream&);
				uint8_t in8(Port);
				uint16_t in16( );
				void out8(Port, uint8_t);
			private:
				HAL *mWrappedHAL;
				std::ostream& mOS;
				lib::IOStreamFormat mIOFormat;
		};
	}
}

#endif

