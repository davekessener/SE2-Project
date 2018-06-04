#include <iostream>
#include <atomic>

#include "lib/arguments.h"

#include "hal/physical.h"

#include "hal/buttons.h"
#include "hal/height_sensor.h"
#include "hal/motor.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"

#include "base/config_object.h"

#include "test/ft/heightsensor/item_measurement.h"

#include "lib/timer.h"

namespace esep { namespace test { namespace functional {



void testHeightSensor(const lib::Arguments& args)
{
	h::ItemMeasurement measure;

	measure.run();
}

}}}
