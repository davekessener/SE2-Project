#ifndef ESEP_QNX_PULSE_H
#define ESEP_QNX_PULSE_H

#include "lib/utils.h"

namespace esep
{
	namespace qnx
	{
		struct pulse_t
		{
			int8_t code;
			uint32_t value;

			pulse_t( ) : code(0), value(0) { }
			pulse_t(int8_t c, uint32_t v) : code(c), value(v) { }
			template<typename C, typename V>
				pulse_t(C&& c, V&& v) : code(static_cast<int8_t>(c)), value(static_cast<uint32_t>(v)) { }
		};

		static constexpr int INVALID_ID = -1;

		typedef int channel_id_t;
		typedef int connection_id_t;
	}
}

#endif
