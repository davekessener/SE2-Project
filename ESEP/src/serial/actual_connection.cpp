
#include <fcntl.h>
#include <serial/actual_connection.h>
#include <termios.h>
#include <iostream>

#define BAUDRATE	19200 // Speed of connection in bits/s

namespace esep { namespace serial {

ActualConnection::ActualConnection(const std::string& device)
: fildes_(0)
{
	fildes_ = open(device.c_str(),O_RDWR);

	if(this->fildes_ == -1)
	{
		throw lib::stringify("Could not connect to device '", device, "'!");
	}

	struct termios ts;
	tcflush(this->fildes_, TCIOFLUSH);
	tcgetattr(this->fildes_, &ts);
	cfsetispeed(&ts, BAUDRATE);
	cfsetospeed(&ts, BAUDRATE);
	ts.c_cflag &= ~CSIZE;
	ts.c_cflag &= ~CSTOPB;
	ts.c_cflag &= ~PARENB;
	ts.c_cflag |= CS8;
	ts.c_cflag |= CREAD;
	ts.c_cflag |= CLOCAL;
	tcsetattr(this->fildes_, TCSANOW, &ts);
}

ActualConnection::~ActualConnection()
{
    if(this->fildes_ != -1) {
    	if (close(this->fildes_) == -1)
    	{
    		throw lib::stringify("Could not close device!");
    	}
    }
}

void ActualConnection::write(const byte_t* buffer, size_t size)
{
	if(this->fildes_ != -1)
	{
		throw lib::stringify("Connection to device is closed!");
	}

    if(::write(fildes_, buffer, size) == -1)
    {
    	throw lib::stringify("An error occured writing to device!");
    }
}

void ActualConnection::read(byte_t* buffer, size_t size)
{
	if(this->fildes_ != -1)
	{
		throw lib::stringify("Connection to device is closed!");
	}

    if (readcond(this->fildes_, buffer, size, size, 0, 10000) == -1)
    {
    	throw lib::stringify("Could not read from device!");
    }
}

void ActualConnection::open(const std::string& device)
{
	// TODO
}

void ActualConnection::close(void)
{
	// TODO
}

bool ActualConnection::isOpen(void) const
{
	// TODO

	return false;
}

}}
