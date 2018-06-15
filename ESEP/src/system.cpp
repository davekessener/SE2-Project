#include <atomic>
#include <iostream>

#include "system.h"

#include "hal.h"

#include "hal/physical.h"
#include "lib/logger.h"
#include "lib/timer.h"

#include "base/dummy_master.h"
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
	typedef hal::HAL::Event Event;

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
	base::Handler handler(&mConfig);
	master::plugin::Hausdorff::processor_t analyser;

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
		auto m = new communication::Master(&handler, std::move(serial));

		com.reset(m);
		master.reset(new master::Master(com.get(), [](const master::Item& item) {
			HAL_CONSOLE.println("Item reached end. ID is ", item.ID());
		}));

//		master->add(master::Plugin_ptr(new SimplePlugin));
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

		m->setMaster(master.get());
	}
	else
	{
		com.reset(new communication::Slave(&handler, std::move(serial)));
	}

	handler.setMaster(com.get());

	mHAL->setCallback([&handler](Event e) {
		handler.handle(e);
	});

	MXT_LOG_INFO("Successfully connected. Starting main program now.");
	HAL_CONSOLE.println("Starting main program");

	while(handler.running())
	{
		lib::Timer::instance().sleep(10);
	}

	mHAL->setCallback([](Event e) { });

	MXT_LOG_INFO("End of system::run.");
}

}}

