#ifndef ESEP_TEST_FT_HAL_H
#define ESEP_TEST_FT_HAL_H

#include "lib/utils.h"
#include "lib/tuple.h"

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
				typedef hal::Buttons::Button Button;
				typedef hal::LEDs::LED LED;
				typedef hal::LightBarriers::LightBarrier LightBarrier;
				typedef hal::Lights::Light Light;
				typedef hal::HAL::Event Event;

				typedef lib::Tuple<tml::MakeTypeList<
					hal::Buttons,
					hal::HeightSensor,
					hal::LEDs,
					hal::Switch,
					hal::LightBarriers,
					hal::Lights,
					hal::MetalSensor,
					hal::Motor
				>> tuple_t;

				struct Cleaner
				{
					Cleaner(tuple_t& t) : hal(t) { }
					~Cleaner( );
					tuple_t& hal;
				};

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
					template<typename T>
					T& get( )
					{
						return mHALInterfaces.get<T>();
					}

				private:
					std::vector<test_fn> mTests;
					std::vector<lib::Timer::Class::id_t> mTimers;
					hal::HAL *mHAL;
					tuple_t mHALInterfaces;
			};
		}
	}
}

#endif
