#include "lib/timer/base.h"

#include "lib/logger.h"

namespace esep { namespace timer {

void Base::tick(void)
{
	if(!done())
	{
		if(!mRemaining--)
		{
			bool has_next = false;

			try
			{
				execute();

				has_next = true;
			}
			MXT_CATCH_ALL_STRAY

			if(has_next && mPeriod)
			{
				mRemaining = mPeriod - 1;
			}
			else
			{
				mActive = false;
			}
		}
	}
}

}}
