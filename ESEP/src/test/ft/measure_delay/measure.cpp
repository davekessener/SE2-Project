#include "test/ft/measure_delay.h"

#include "hal/physical.h"

#include "hal.h"

namespace esep { namespace test { namespace functional {

void measureDelay(const lib::Arguments&)
{
	typedef hal::LightBarriers::LightBarrier LightBarrier;
	typedef hal::HAL::Event Event;

	base::ConfigObject config;
	hal::Physical hal;

	uint64_t ts = 0, o = 0;
	uint inter = 0;

	HAL::instance().instantiate(&hal, &config);

	enum class State { FIRST, BACK, SECOND, END };

	State s = State::FIRST;

	hal.setCallback([&](Event e) {
		if(e == Event::BTN_STOP)
		{
			s = State::END;
		}

		switch(s)
		{
		case State::FIRST:
			switch(e)
			{
			case Event::LB_START:
				if(HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
				{
					HAL_MOTOR.start();
					HAL_SWITCH.open();
				}
				else
				{
					ts = lib::Timer::instance().elapsed();
				}
				break;

			case Event::LB_END:
				if(HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
				{
					o = lib::Timer::instance().elapsed() - ts;
					s = State::BACK;
					HAL_CONSOLE.println("Fast: ", o);
					HAL_MOTOR.left();
				}
				break;

			default:
				break;
			}
			break;

		case State::BACK:
			switch(e)
			{
			case Event::LB_START:
				if(HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
				{
					HAL_MOTOR.right();
					s = State::SECOND;
				}
				break;

			default:
				break;
			}
			break;

		case State::SECOND:
			switch(e)
			{
			case Event::LB_START:
				if(!HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
				{
					ts = lib::Timer::instance().elapsed();
				}
				break;

			case Event::LB_END:
				if(HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
				{
					auto d = lib::Timer::instance().elapsed() - ts;
					HAL_MOTOR.stop();
					HAL_SWITCH.close();
					HAL_CONSOLE.println("With ", inter, " interruptions: ", d, " (", (d - o) / inter, ")");
					s = State::END;
				}
				break;

			default:
				break;
			}
			break;

			default:
				break;
		}
	});

#define SLP 500
	while(s != State::END)
	{
		lib::Timer::instance().sleep(SLP);

		if(s == State::SECOND)
		{
			auto t = lib::Timer::instance().elapsed();
			HAL_MOTOR.stop();
			lib::Timer::instance().sleep(SLP);
			HAL_MOTOR.start();
			ts += (lib::Timer::instance().elapsed() - t);
			++inter;
		}
	}

	HAL::instance().clear();
}

}}}
