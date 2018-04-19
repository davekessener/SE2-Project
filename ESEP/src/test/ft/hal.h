#ifndef ESEP_TEST_FT_HAL_H
#define ESEP_TEST_FT_HAL_H

#include "lib/utils.h"
#include "lib/timer.h"

#include "hal/hal.h"
#include "hal/buttons.h"
#include "hal/height_sensor.h"
#include "hal/leds.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"
#include "hal/metal_sensor.h"
#include "hal/motor.h"
#include "hal/switch.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			class HALTester
			{
				typedef void (HALTester::*test_fn)(hal::HAL::Event);
				typedef lib::Timer::Class::id_t timer_id_t;

				typedef hal::Buttons::Button Button;
				typedef hal::LEDs::LED LED;
				typedef hal::LightBarriers::LightBarrier LightBarrier;
				typedef hal::Lights::Light Light;
				typedef hal::HAL::Event Event;

				static constexpr timer_id_t INVALID_TIMER_ID = lib::Timer::Class::INVALID_TIMER_ID;

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
					void t_010(Event);
					void t_011(Event);
					void clearTimers( );
					void resetHAL( );

					template<typename F>
					void runIn(uint t, F&& f)
					{
						mTimers.push_back(lib::Timer::instance().registerCallback(std::forward<F>(f), t));
					}

				private:
					std::vector<test_fn> mTests;
					std::vector<timer_id_t> mTimers;
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
