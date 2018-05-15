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

namespace esep { namespace system {

Impl::Impl(void)
	: mHAL(new hal::Physical())
	, mHALObjects(
		hal::Buttons(mHAL),
		hal::HeightSensor(mHAL),
		hal::LEDs(mHAL),
		hal::Switch(mHAL),
		hal::LightBarriers(mHAL),
		hal::Lights(mHAL),
		hal::MetalSensor(mHAL),
		hal::Motor(mHAL))
{
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
	typedef hal::Lights::Light Light;

	bool is_master = false;

	if(args.get("run") == MXT_MASTER)
	{
		is_master = true;

		std::cout << "Running system as MASTER!" << std::endl;
	}
	else if(args.get("run") == MXT_SLAVE)
	{
		is_master = false;

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

	for(uint i = 0 ; i < 30 ; ++i)
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

	std::cout << "Starting main program" << std::endl;

	while(handler.running())
	{
		lib::Timer::instance().sleep(10);
	}

	get<hal::Switch>().close();
	get<hal::Motor>().right();
	get<hal::Motor>().fast();
	get<hal::Motor>().enable();
	get<hal::Motor>().stop();
	get<hal::Lights>().turnOff(Light::RED);
	get<hal::Lights>().turnOff(Light::GREEN);
	get<hal::Lights>().turnOff(Light::YELLOW);
}

}}

