#include <fcntl.h>
#include <termios.h>
#include <iostream>

#include "test/ft/serial/qnx.h"

#include "lib/utils.h"
#include "lib/timer.h"

#include "hal/hal.h"
#include "hal/physical.h"
#include "hal/lights.h"

#define MXT_BAUDRATE 19200

namespace esep { namespace test { namespace functional {

int open_serial(const std::string& device)
{
	int f = ::open(device.c_str(), O_RDWR);

	if(f == -1)
	{
		MXT_THROW("Could not connect to device '", device, "'!");
	}

	struct termios ts;

	tcflush(f, TCIOFLUSH);
	tcgetattr(f, &ts);

	cfsetispeed(&ts, MXT_BAUDRATE);
	cfsetospeed(&ts, MXT_BAUDRATE);

	ts.c_cflag &= ~CSIZE;
	ts.c_cflag &= ~CSTOPB;
	ts.c_cflag &= ~PARENB;
	ts.c_cflag |= CS8;
	ts.c_cflag |= CREAD;
	ts.c_cflag |= CLOCAL;

	tcsetattr(f, TCSANOW, &ts);

	return f;
}

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

void test_serial_basic(void)
{
	HAL myHAL;

	hal::Lights lights(myHAL.hal);

	std::string msg("Hello, World!");

	std::cout << "Am I reader or writer? (0/1)\n";

	int type = 0;

	std::cin >> type;

	int f = open_serial("/dev/ser4");

	std::cout << "File descriptor is " << f << std::endl;

	switch(type)
	{
	case 0:
		{
			std::vector<byte_t> buf;
			byte_t v;
			uint has_read = 0;

			while(has_read < msg.size())
			{
				auto r = readcond(f, &v, sizeof(v), sizeof(v), 0, 1);

				if(r == -1)
				{
					MXT_THROW("Error in read!");
				}
				else if(r == 0)
				{
					continue;
				}

				std::cout << "I have read a byte: " << lib::hex<8>(v) << "\n";

				has_read += r;
				buf.push_back(v);
			}

			std::string s(std::string(buf.cbegin(), buf.cend()));

			if(msg == s)
			{
				std::cout << "SUCCESS!\n";
			}
			else
			{
				std::cout << "FAILURE! (got '" << s << ")\n";
			}
		}
		break;
	case 1:
		std::cout << "Writing " << msg.size() << " chars\n";
		write(f, msg.c_str(), msg.size());
		break;
	default:
		MXT_THROW("ERROR");
		break;
	}

	std::cin.ignore();
	std::string line;
	std::getline(std::cin, line);

	close(f);

	std::cout << "Goodbye" << std::endl;
}

}}}
