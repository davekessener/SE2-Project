#ifndef SRC_BASE_RUN_TYPES_H_
#define SRC_BASE_RUN_TYPES_H_

#include "lib/utils.h"

namespace esep
{
	namespace base
	{
		namespace run
		{
			/*
			 * Enumeration for the location of the token
			 */
			enum class State : uint8_t
			{
				IN_LB_START,
				BF_LB_START,
				BW_START_HS,
				BF_LB_HS,
				IN_LB_HS,
				BW_HS_SWITCH,
				BF_LB_SWITCH,
				IN_SWITCH,
				BF_LB_RAMP,
				IN_LB_RAMP,
				IN_SWITCH_KEEP,
				BW_SWITCH_END,
				BF_END,
				IN_LB_END,
				SWITCH_CTRL,
				IN_LB_SWITCH
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
