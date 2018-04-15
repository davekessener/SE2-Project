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
	// TODO Auto-generated constructor stub

}

SerialConnection::~SerialConnection()
{
	// TODO Auto-generated destructor stub
}

void SerialConnection::run()
{
	bool running = true;
	char mode;

	MXT_LOG("Connecting to other module...");
	mConnection = new serial::ActualConnection("/dev/ser4");
	mClient = new serial::Client(*mConnection);
	MXT_LOG("Connected!");

	while (running)
	{
		MXT_LOG("Press '1' to send, '2' to receive or 'q' to quit");
		std::cin >> mode;

		if (mode == '1')
		{
			MXT_LOG("You are the sender! Please write a message:");
			std::string msg;
			std::getline(std::cin, msg);
			serial::Client::buffer_t buf(msg.cbegin(), msg.cend());
			mClient->write(buf);
			MXT_LOG("Message was sent!");
		}
		else if (mode == '2')
		{
			MXT_LOG("You are the receiver! Waiting for message...");
			serial::Client::buffer_t r = mClient->read();
			std::string result(r.begin(), r.end());
			std::cout << "Got message: " << result << std::endl;
		}
		else if ( mode == 'q')
		{
			running = false;
		}
	}

	MXT_LOG("Bye bye!");

}

}}}

