/*
 * serial_connection.cpp
 *
 *  Created on: 15.04.2018
 *      Author: Martin
 */

#include <iostream>

#include "test/ft/serial/bsp.h"

#include "lib/logger.h"

#include "serial/bsp_client.h"
#include "serial/watchdog.h"


#define MXT_SERIAL_DEVICE "/dev/ser4"


namespace esep { namespace test { namespace functional {

SerialConnectionTest::SerialConnectionTest()
{
	std::cout << "START OF SERIAL TEST\nConnecting to other module " << std::flush;

	connection_ptr c(new serial::ActualConnection(MXT_SERIAL_DEVICE));
	mClient.reset(new serial::BSPClient(std::move(c)));

	std::cout << "\nConnected! " << std::endl;
}

SerialConnectionTest::~SerialConnectionTest(void)
{
	std::cout << "END OF SERIAL TEST" << std::endl;
}

void SerialConnectionTest::run()
{
	try
	{
		std::string line;

		while(true)
		{
			std::cout << "Press '1' to send, '2' to receive or 'q' to quit" << std::endl;
			std::getline(std::cin, line);

			if(line.empty())
			{
				continue;
			}
			else if(line[0] == '1')
			{
				std::cout << "You are the sender!\nPlease write a message: ";
				std::getline(std::cin, line);

				mClient->write(buffer_t(line.cbegin(), line.cend()));

				std::cout << "Message '" << line << "' (" << line.size() << ") was sent!" << std::endl;
			}
			else if(line[0] == '2')
			{
				std::cout << "You are the receiver!\nWaiting for message ..." << std::endl;

				buffer_t r = mClient->read();

				std::cout << "Got message: '" << std::string(r.begin(), r.end()) << "' (" << r.size() << ")" << std::endl;
			}
			else if(line[0] == 'q')
			{
				break;
			}
		}
	}
	catch(const ConnectionClosedException& e)
	{
		std::cout << "\nConnection was terminated!" << std::endl;
	}
}

}}}

