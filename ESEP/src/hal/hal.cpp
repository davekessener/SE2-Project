#include "hal/hal.h"

namespace esep { namespace hal {

const HAL::Event HAL::EVENTS[] = {
	Event::LB_START, Event::LB_END, Event::LB_HEIGHTSENSOR, Event::LB_RAMP, Event::LB_SWITCH,
	Event::BTN_START, Event::BTN_STOP, Event::BTN_RESET, Event::BTN_ESTOP
};

}}
