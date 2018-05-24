#ifndef SRC_BASE_RUN_TYPES_H_
#define SRC_BASE_RUN_TYPES_H_

#include "lib/utils.h"

namespace esep
{
	namespace base
	{
		namespace run
		{
			typedef uint32_t id_t;

			enum class TimerEvent : uint8_t
			{
				EXPECT_NEW,
				START_1,
				ITEM_READY_HS,
				START_2,
				HS_1,
				ITEM_READY_SWITCH,
				HS_2,
				SWITCH_1,
				RAMP,
				SWITCH_2,
				ITEM_READY_END,
				SWITCH_3,
			};
		}
	}
}

#endif /* SRC_BASE_RUN_TYPES_H_ */
