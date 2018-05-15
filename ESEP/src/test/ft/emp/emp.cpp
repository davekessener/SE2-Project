#include <iostream>

#include "test/ft/emp.h"

#include "hal/hal.h"
#include "hal/physical.h"
#include "hal/motor.h"
#include "hal/switch.h"
#include "hal/buttons.h"
#include "hal/light_barriers.h"

#include "emp/recorder.h"
#include "emp/playback.h"

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/reader.h"
#include "lib/writer.h"

#include "lib/io/file_reader.h"
#include "lib/io/file_writer.h"

#define MXT_FN "hal.emp"

namespace esep { namespace test { namespace functional {

namespace
{

inline hal::HAL *make_playback( ) { return new emp::Playback(lib::Reader_ptr(new lib::ImmediateFileReader(MXT_FN)), hal::HAL_ptr(new hal::Physical)); }
inline hal::HAL *make_recorder( ) { return new emp::Recorder(lib::Writer_ptr(new lib::FileWriter(MXT_FN)), hal::HAL_ptr(new hal::Physical)); }

struct Tester
{
	typedef std::unique_ptr<hal::HAL> HAL_ptr;
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;
	typedef hal::LightBarriers::LightBarrier LightBarrier;

	Tester(bool playback)
		: hal(playback ? make_playback() : make_recorder())
		, running(true)
		, MOTOR(hal.get())
		, SWITCH(hal.get())
		, BUTTONS(hal.get())
		, LBS(hal.get())
	{
	}

	~Tester( )
	{
		SWITCH.close();
		MOTOR.right();
		MOTOR.fast();
		MOTOR.enable();
		MOTOR.stop();
	}

	void run(void)
	{
		hal->setCallback([this](Event e) {
			MXT_LOG(lib::stringify("Got event ", lib::hex<32>(hal::HAL::getPin(e)), " from GPIO ", hal::HAL::getField(e)));

			switch(e)
			{
			case Event::LB_START:
				if(LBS.isBroken(LightBarrier::LB_START))
				{
					MOTOR.right();
					MOTOR.fast();
				}
				break;
			case Event::LB_SWITCH:
				if(LBS.isBroken(LightBarrier::LB_SWITCH))
				{
					SWITCH.open();
				}
				else
				{
					mTimer = lib::Timer::instance().registerCallback([this](void) {
						SWITCH.close();
					}, 500);
				}
				break;
			case Event::LB_END:
				if(LBS.isBroken(LightBarrier::LB_END))
				{
					MOTOR.stop();
				}
				break;
			case Event::BTN_STOP:
				if(BUTTONS.isPressed(Button::STOP))
				{
					running = false;
				}
				break;
			default:
				break;
			}
		});

		while(running.load())
		{
			lib::Timer::instance().sleep(10);
		}
	}

	HAL_ptr hal;
	std::atomic<bool> running;
	hal::Motor MOTOR;
	hal::Switch SWITCH;
	hal::Buttons BUTTONS;
	hal::LightBarriers LBS;
	timer::Manager mTimer;
};

}

void testEMP(const lib::Arguments& args)
{
	std::cout << "=== EMP TEST ===========================================================" << std::endl;

	lib::Timer::instance().reset();
	Tester t(args.has("play"));

	t.run();

	std::cout << "========================================================================" << std::endl;
}

}}}
