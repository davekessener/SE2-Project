#ifndef ESEP_QNX_PULSE_H
#define ESEP_QNX_PULSE_H

#include "lib/utils.h"

namespace esep
{
	namespace qnx
	{
		struct pulse_t
		{
			uint8_t code;
			uint32_t value;
		};

		typedef int channel_id_t;
		typedef int connection_id_t;
	}
}

#endif
