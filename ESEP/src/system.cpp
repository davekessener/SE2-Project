#include <atomic>
#include <iostream>

#include "system.h"

#include "hal.h"

#include "hal/physical.h"
#include "lib/logger.h"
#include "lib/timer.h"

#include "base/handler.h"

#include "master/master.h"
#include "master/plugin/items.h"

#include "communication/base.h"
#include "communication/master.h"
#include "communication/slave.h"

#include "serial/connection.h"
#include "serial/client.h"
#include "serial/actual_connection.h"
#include "serial/bsp_client.h"
#include "serial/watchdog.h"

#define MXT_MASTER "master"
#define MXT_SLAVE "slave"

#define MXT_DEFAULT_DEVICE "/dev/ser4"
#define MXT_SERIAL_TIMEOUT 60

namespace esep { namespace system {

namespace
{
	class SimplePlugin : public master::Plugin
	{
		public:
			SimplePlugin( ) : Plugin(Type::UNKNOWN) { }
			float match(const data_t&) override { return 1.0; }
			Action decide(const history_t&) override { return Action::KEEP; }
	};
}

typedef hal::Buttons::Button Button;

Impl::Impl(void)
	: mHAL(new hal::Physical())
{
	HAL::instance().instantiate(mHAL, &mConfig);

	MXT_LOG_INFO("Creating system object!");
}

Impl::~Impl(void)
{
	delete mHAL;
}

void Impl::run(const lib::Arguments& args)
{
	typedef std::unique_ptr<serial::Connection> Connection_ptr;
	typedef std::unique_ptr<serial::Client> Client_ptr;
	typedef hal::LEDs::LED LED;
	typedef hal::HAL::Event Event;

	MXT_LOG_INFO("==========================================================================================================");

	if(HAL_BUTTONS.isPressed(Button::ESTOP))
	{
		MXT_LOG_FATAL("Can't start system when ESTOP is active!");

		return;
	}

	bool is_master = false;

	if(args.get("run") == MXT_MASTER)
	{
		is_master = true;

		MXT_LOG_INFO("Starting system in MASTER mode.");

		HAL_CONSOLE.println("Running system as MASTER!");
	}
	else if(args.get("run") == MXT_SLAVE)
	{
		is_master = false;

		MXT_LOG_INFO("Starting system in SLAVE mode.");

		HAL_CONSOLE.println("Running system as SLAVE!");
	}
	else
	{
		MXT_THROW_EX(InvalidRunArgumentException);
	}

	lib::Timer::instance().reset();

	std::unique_ptr<master::Master> master;
	std::unique_ptr<communication::Base> com;
	master::plugin::Hausdorff::processor_t analyser;
	uint16_t hs = 0;

	Connection_ptr c(new serial::ActualConnection(MXT_DEFAULT_DEVICE));
	Client_ptr bsp(new serial::BSPClient(std::move(c)));
	Client_ptr serial(new serial::Watchdog(std::move(bsp)));

	HAL_CONSOLE.print("Connecting via serial ");

	for(uint i = 0 ; i < MXT_SERIAL_TIMEOUT ; ++i)
	{
		HAL_CONSOLE.print(".");
		lib::Timer::instance().sleep(1000);
		if(serial->connected()) break;
	}
	HAL_CONSOLE.println();

	if(!serial->connected())
	{
		auto e = "Serial connection failed! Shutting down ...";

		MXT_LOG_FATAL(e);

		HAL_CONSOLE.println(e);

		MXT_THROW_EX(ConnectionException);
	}

	lib::Timer::instance().reset();

	if(is_master)
	{
		com.reset(lib::with_temporary(new communication::Master(std::move(serial)), [&](communication::Master *m) {
			master.reset(new master::Master(&analyser, m, [](const master::Item& item) {
				HAL_CONSOLE.println("Item reached end. ID is ", item.ID());
			}));

			m->setMaster(master.get());

			return m;
		}));

		master->add(master::Plugin_ptr(new master::plugin::Coded_000(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Coded_001(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Coded_010(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Coded_011(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Coded_100(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Coded_101(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Coded_110(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Coded_111(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Flat(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::UpsideDown(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::Hollow(&analyser)));
		master->add(master::Plugin_ptr(new master::plugin::HollowMetal(&analyser)));
	}
	else
	{
		com.reset(new communication::Slave(std::move(serial)));
	}

	base::Handler handler(com.get(), &mConfig);

	com->setBase(&handler);

	mHAL->setCallback([&handler, &hs](Event e) {
		if(e == Event::HEIGHT_SENSOR)
		{
			uint16_t n = HAL_HEIGHT_SENSOR.measure();

			if(n != hs)
			{
				handler.handle(e);
			}

			hs = n;
		}
		else
		{
			handler.handle(e);
		}
	});

	MXT_LOG_INFO("Successfully connected. Starting main program now.");

	HAL_CONSOLE.println("Starting main program");
	HAL_LEDS.turnOn(LED::START);

	while(handler.running())
	{
		lib::Timer::instance().sleep(10);
	}

	mHAL->setCallback([](Event e) { });

	MXT_LOG_INFO("End of system::run.");
}

}}

