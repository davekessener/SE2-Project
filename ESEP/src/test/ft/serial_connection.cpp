/*
 * serial_connection.cpp
 *
 *  Created on: 15.04.2018
 *      Author: Martin
 */

#include <iostream>
#include "serial_connection.h"
#include "lib/logger.h"

namespace esep { namespace test { namespace functional {

SerialConnection::SerialConnection()
{
	std::cout << "Connecting to other module..." << std::endl;
	mConnection = new serial::ActualConnection("/dev/ser4");
	mClient = new serial::Client(*mConnection);
	std::cout << "Connected! " << std::endl;

}

SerialConnection::~SerialConnection()
{
	delete mClient;
	delete mConnection;
}

void SerialConnection::run()
{
	bool running = true;
	char mode;

	while (running)
	{
		std::cout << "Press '1' to send, '2' to receive or 'q' to quit" << std::endl;
		std::cin >> mode;

		if (mode == '1')
		{
			std::string msg;
			std::cout << "You are the sender! Please write a message: ";
			std::cin.ignore();
			std::getline(std::cin, msg);
			serial::Client::buffer_t buf(msg.cbegin(), msg.cend());
			mClient->write(buf);
			std::cout << "Message '" << msg << "' was sent!" << std::endl;
		}
		else if (mode == '2')
		{
			std::cout << "You are the receiver! Waiting for message..." << std::endl;
			serial::Client::buffer_t r = mClient->read();
			std::string result(r.begin(), r.end());
			std::cout << "Got message: " << result << std::endl;
		}
		else if ( mode == 'q')
		{
			running = false;
		}
	}

	std::cout << "Bye bye!" <<std::endl;

}

}}}

