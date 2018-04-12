#ifndef ESEP_QNX_PULSE_H
#define ESEP_QNX_PULSE_H

#include "lib/utils.h"

namespace esep
{
	namespace qnx
	{
		enum class Code : int8_t
		{
			SHUTDOWN,
			INTERRUPT,
			GPIO_1_OUT,
			GPIO_1_SET,
			GPIO_1_RESET,
			GPIO_2_OUT,
			GPIO_2_SET,
			GPIO_2_RESET
		};

		struct pulse_t
		{
			int8_t code;
			uint32_t value;

			pulse_t( ) : code(0), value(0) { }
			pulse_t(int8_t c, uint32_t v) : code(c), value(v) { }
			pulse_t(Code c, uint32_t v) : code(static_cast<uint32_t>(c)), value(v) { }
		};

		static constexpr int INVALID_ID = -1;

		typedef int channel_id_t;
		typedef int connection_id_t;
	}
}

#endif
