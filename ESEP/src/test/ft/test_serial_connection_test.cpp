#include <test/ft/serial_connection_test.h>
#include <iostream>
#include <vector>

#include "hal/hal.h"
#include "hal/physical.h"
#include "hal/lights.h"

#include "lib/timer.h"

#include "serial/connection.h"
#include "serial/actual_connection.h"

namespace esep { namespace test { namespace functional {

struct HAL
{
	HAL( ) : hal(new hal::Physical), lights(hal)
	{
		lib::Timer::instance().sleep(100);
		lights.turnOn(hal::Lights::Light::RED);
	}
	~HAL( ) { lights.turnOff(hal::Lights::Light::RED); delete hal; }
	hal::HAL *hal;
	hal::Lights lights;
};

void test_serial_connection_class(void)
{
	HAL myHAL;

	std::string msg("Hello, World!");
	std::vector<byte_t> buf(msg.size());

	std::cout << "Am I sender or receiver? (0/1)\n";

	std::shared_ptr<serial::Connection> connection_p(new serial::ActualConnection("/dev/ser4"));

	uint type = 0;

	std::cin >> type;

	switch(type)
	{
	case 0:
		std::cout << "I am sender!\nSending " << msg.size() << " bytes!\n";
		connection_p->write(reinterpret_cast<const byte_t *>(msg.c_str()), msg.size());
		break;
	case 1:
	{
		std::cout << "I am receiver!\nReading " << buf.size() << " bytes!\n";
		connection_p->read(&buf.front(), buf.size());
		std::string s(buf.cbegin(), buf.cend());
		if(msg == s)
		{
			std::cout << "SUCCESS!\n";
		}
		else
		{
			std::cout << "FAILURE! (received '" << s << "'!)\n";
		}
	}
		break;
	default:
		MXT_THROW("ERROR");
		break;
	}

	std::cin.ignore();
	std::string line;
	std::getline(std::cin, line);

	std::cout << "Goodbye" << std::endl;
}

}}}
