#include "base/error/base.h"

#include "system.h"

namespace esep { namespace base { namespace error {

void Base::enter(void)
{
	HAL_MOTOR.stop();
	HAL_SWITCH.close();
}

}}}
