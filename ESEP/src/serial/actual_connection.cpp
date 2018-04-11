
#include <fcntl.h>
#include <serial/actual_connection.h>
#include <termios.h>
#include <iostream>

#define BAUDRATE	19200 // Speed of connection in bits/s

namespace esep { namespace serial {

ActualConnection::ActualConnection(const std::string& device)
: fildes_(0)
{
	open(device);
}

ActualConnection::~ActualConnection()
{
    close();
}

void ActualConnection::write(const byte_t* buffer, size_t size)
{
	if(!isOpen())
	{
		MXT_THROW("Connection to device is closed!");
	}

    if(::write(fildes_, buffer, size) != (ssize_t) size)
    {
    	MXT_THROW("An error occured writing to device!");
    }
}

void ActualConnection::read(byte_t* buffer, size_t size)
{
	size_t bytes_read = 0;
	int return_value = 0;

	while(bytes_read != size)
	{
		if(!isOpen())
		{
			MXT_THROW("Connection to device is closed!");
		}

		return_value = readcond(this->fildes_, buffer+bytes_read, size-bytes_read, size-bytes_read, 0, 5); // timeout is specified with 1/10 of a second

		if (return_value == -1)
		{
			MXT_THROW("Could not read from device!");
		}

		bytes_read += return_value;
	}
}

void ActualConnection::open(const std::string& device)
{
	close(); // Ensure that connection is closed before opening a new one

	fildes_ = ::open(device.c_str(),O_RDWR);

	if(this->fildes_ == -1)
	{
		MXT_THROW("Could not connect to device '", device, "'!");
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

void ActualConnection::close(void)
{
	if(isOpen()) {
		if (::close(this->fildes_) == -1)
		{
			MXT_THROW("Could not close device!");
		}
	}
}

bool ActualConnection::isOpen(void) const
{
	return this->fildes_ != -1;
}

}}
