#include "base/error/base.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

void Base::enter(void)
{
	HAL_MOTOR.stop();
	HAL_SWITCH.close();
}

}}}
