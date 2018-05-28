#ifndef SRC_BASE_RUN_TYPES_H_
#define SRC_BASE_RUN_TYPES_H_

#include "lib/utils.h"

namespace esep
{
	namespace base
	{
		namespace run
		{
			enum class State : uint8_t
			{
				STATE_1,
				STATE_2,
				STATE_3,
				STATE_4,
				STATE_5,
				STATE_6,
				STATE_7,
				STATE_8,
				STATE_9,
				STATE_10,
				STATE_11,
				STATE_12,
				STATE_13,
				STATE_14,
			};



			enum class HalEvent : uint8_t
			{
				LB_START,
				I_LB_START,
				LB_HS,
				I_LB_HS,
				LB_SWITCH,
				I_LB_SWITCH,
				LB_RAMP,
				I_LB_RAMP,
				LB_END,
				I_LB_END,
			};
		}
	}
}

#endif /* SRC_BASE_RUN_TYPES_H_ */
