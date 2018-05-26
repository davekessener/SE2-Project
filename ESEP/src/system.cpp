#include <atomic>
#include <iostream>

#include "system.h"

#include "hal/physical.h"
#include "lib/logger.h"
#include "lib/timer.h"

#include "base/dummy_master.h"
#include "base/handler.h"

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

typedef hal::Buttons::Button Button;

Impl::Impl(void)
	: mHAL(new hal::Physical())
	, mHALObjects(
		hal::Buttons(mHAL),
		hal::HeightSensor(mHAL, &mConfig),
		hal::LEDs(mHAL),
		hal::Switch(mHAL),
		hal::LightBarriers(mHAL),
		hal::Lights(mHAL),
		hal::MetalSensor(mHAL),
		hal::Motor(mHAL))
{
	MXT_LOG_INFO("Creating system object!");
}

Impl::~Impl(void)
{
	get<hal::Switch>().close();
	get<hal::Motor>().stop();

	delete mHAL;
}

void Impl::run(const lib::Arguments& args)
{
	typedef std::unique_ptr<serial::Connection> Connection_ptr;
	typedef std::unique_ptr<serial::Client> Client_ptr;

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

		std::cout << "Running system as MASTER!" << std::endl;
	}
	else if(args.get("run") == MXT_SLAVE)
	{
		is_master = false;

		MXT_LOG_INFO("Starting system in SLAVE mode.");

		std::cout << "Running system as SLAVE!" << std::endl;
	}
	else
	{
		MXT_THROW_EX(InvalidRunArgumentException);
	}

	lib::Timer::instance().reset();

	std::unique_ptr<communication::IRecipient> master;
	std::unique_ptr<communication::Base> com;
	base::Handler handler;

	Connection_ptr c(new serial::ActualConnection(MXT_DEFAULT_DEVICE));
	Client_ptr bsp(new serial::BSPClient(std::move(c)));
	Client_ptr serial(new serial::Watchdog(std::move(bsp)));

	std::cout << "Connecting via serial " << std::flush;

	for(uint i = 0 ; i < MXT_SERIAL_TIMEOUT ; ++i)
	{
		std::cout << "." << std::flush;
		lib::Timer::instance().sleep(1000);
		if(serial->connected()) break;
	}
	std::cout << std::endl;

	if(!serial->connected())
	{
		MXT_LOG_FATAL("Serial connection failed! Shutting down ...");

		MXT_THROW_EX(ConnectionException);
	}

	lib::Timer::instance().reset();

	if(is_master)
	{
		auto m = new communication::Master(&handler, std::move(serial));

		com.reset(m);
		master.reset(new base::DummyMaster(com.get()));

		m->setMaster(master.get());
	}
	else
	{
		com.reset(new communication::Slave(&handler, std::move(serial)));
	}

	handler.setMaster(com.get());

	mHAL->setCallback([&handler](hal::HAL::Event e) {
		handler.handle(e);
	});

	MXT_LOG_INFO("Successfully connected. Starting main program now.");
	std::cout << "Starting main program" << std::endl;

	while(handler.running())
	{
		lib::Timer::instance().sleep(10);
	}

	MXT_LOG_INFO("End of system::run.");
}

}}

