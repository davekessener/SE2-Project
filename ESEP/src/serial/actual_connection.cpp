#include <fcntl.h>
#include <serial/actual_connection.h>
#include <termios.h>
#include <iostream>

#define BAUDRATE	19200 // Speed of connection in bits/s

namespace esep { namespace serial {

ActualConnection::ActualConnection(const std::string& device)
	: mFD(INVALID_FD)
{
	open(device);
}

ActualConnection::~ActualConnection()
{
    close();
}

void ActualConnection::write(const byte_t* b, size_t l)
{
	if(!isOpen())
	{
		MXT_THROW("Connection to device is closed!");
	}

    if(::write(mFD, b, l) != (ssize_t) l)
    {
    	MXT_THROW("An error occured writing to device!");
    }
}

void ActualConnection::read(byte_t* b, size_t size)
{
	size_t bytes_read = 0;
	int r = 0;

	while(bytes_read < size)
	{
		if(!isOpen())
		{
			MXT_THROW("Connection to device is closed!");
		}

		if((r = readcond(mFD, b + bytes_read, size - bytes_read, 1, 0, 1)) == -1)
		{
			if(!isOpen())
			{
				throw ConnectionClosedException();
			}
			else
			{
				MXT_THROW("Could not read from device!");
			}
		}

		bytes_read += r;
	}
}

void ActualConnection::open(const std::string& device)
{
	close(); // Ensure that connection is closed before opening a new one

	if((mFD = ::open(device.c_str(), O_RDWR)) == -1)
	{
		MXT_THROW("Could not connect to device '", device, "'!");
	}

	struct termios ts;

	tcflush(mFD, TCIOFLUSH);
	tcgetattr(mFD, &ts);

	cfsetispeed(&ts, BAUDRATE);
	cfsetospeed(&ts, BAUDRATE);

	ts.c_cflag &= ~CSIZE;
	ts.c_cflag &= ~CSTOPB;
	ts.c_cflag &= ~PARENB;
	ts.c_cflag |= CS8;
	ts.c_cflag |= CREAD;
	ts.c_cflag |= CLOCAL;

	tcsetattr(mFD, TCSANOW, &ts);
}

void ActualConnection::close(void)
{
	if(isOpen())
	{
		int fd = INVALID_FD;

		std::swap(fd, mFD);

		if(::close(fd) == -1)
		{
			MXT_THROW("Could not close device!");
		}
	}
}

bool ActualConnection::isOpen(void) const
{
	return mFD != INVALID_FD;
}

}}
