#ifndef ESEP_TEST_FUNCTIONAL_HAL_TESTER_H
#define ESEP_TEST_FUNCTIONAL_HAL_TESTER_H

#include "test/ft/hal/tester.h"

#include "lib/utils.h"
#include "lib/timer.h"

#include "hal/buttons.h"
#include "hal/height_sensor.h"
#include "hal/leds.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"
#include "hal/metal_sensor.h"
#include "hal/motor.h"
#include "hal/switch.h"

#include "base/config_object.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			class HALTester
			{
				typedef void (HALTester::*test_fn)(hal::HAL::Event);
				typedef timer::Manager timer_t;

				typedef hal::Buttons::Button Button;
				typedef hal::LEDs::LED LED;
				typedef hal::LightBarriers::LightBarrier LightBarrier;
				typedef hal::Lights::Light Light;
				typedef hal::HAL::Event Event;

				public:
					HALTester( );
					~HALTester( );
					void run( );
				private:
					void t_004(Event);
					void t_005(Event);
					void t_006(Event);
					void t_007(Event);
					void t_008(Event);
					void t_009(Event);
					void clearTimers( );
					void resetHAL( );
					void runTest(uint);

					template<typename F>
					void runIn(uint t, F&& f)
					{
						mTimers.emplace_back(lib::Timer::instance().registerCallback(std::forward<F>(f), t));
					}

				private:
					base::ConfigObject mConfig;
					std::vector<std::pair<std::string, test_fn>> mTests;
					bool mRunning;
					std::vector<timer_t> mTimers;
					hal::HAL *mHAL;
					hal::Buttons BUTTONS;
					hal::HeightSensor HEIGHT_SENSOR;
					hal::LEDs LEDS;
					hal::Switch SWITCH;
					hal::LightBarriers LIGHT_BARRIERS;
					hal::Lights LIGHTS;
					hal::MetalSensor METAL_SENSOR;
					hal::Motor MOTOR;
			};
		}
	}
}

#endif
