#ifndef ESEP_TEST

#include "serial/actual_connection.h"

#include <fcntl.h>
#include <termios.h>
#include <iostream>

#define MXT_BAUDRATE	19200 // Speed of connection in bits/s
#define MXT_INVALID_FD	-1

namespace esep { namespace serial {

ActualConnection::ActualConnection(const std::string& device)
: fildes_(MXT_INVALID_FD)
{
	open(device);
}

ActualConnection::~ActualConnection(void)
{
}

void Connection::write(const byte_t* buffer, size_t size)
{
    ::write(fildes_, buffer, size);
}

void Connection::read(byte_t* buffer, size_t size)
{
    ::read(fildes_, buffer, size);
}

void Connection::open(const std::string& device)
{
	close();

	fildes_ = open(device.c_str(), O_RDWR);

	if(fildes_ == MXT_INVALID_FD)
	{
		std::cout << "Could not connect to device: " << device << std::endl;
		exit(EXIT_FAILURE);
	}

	struct termios ts;

	tcflush(fildes_, TCIOFLUSH);
	tcgetattr(fildes_, &ts);

	cfsetispeed(&ts, MXT_BAUDRATE);

	ts.c_cflag &= ~CSIZE;
	ts.c_cflag &= ~CSTOPB;
	ts.c_cflag &= ~PARENB;
	ts.c_cflag |= CS8;
	ts.c_cflag |= CREAD;
	ts.c_cflag |= CLOCAL;

	tcsetattr(fildes_, TCSANOW, &ts);
}

void ActualConnection::close(void)
{
    if(fildes_ != MXT_INVALID_FD)
    {
    	if(close(fildes_) == -1)
    	{
    		std::cout << "Could not close device" << std::endl;
    		exit(EXIT_FAILURE);
    	}
    }
}

}}

#endif
