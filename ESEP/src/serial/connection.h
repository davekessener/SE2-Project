#ifndef ESEP_SERIAL_CONNECTION_H
#define ESEP_SERIAL_CONNECTION_H

#include <string>

#include "lib/utils.h"

namespace esep
{
	namespace serial
	{
		class Connection
		{
			public:
				virtual ~Connection( ) { }
				virtual void write(const byte_t *, size_t) = 0;
				virtual void read(byte_t *, size_t) = 0;
				virtual void open(const std::string&) = 0;
				virtual void close( ) = 0;
			private:
				Connection(const Connection&) = delete;
				Connection(Connection&&) = delete;
				Connection& operator=(const Connection&) = delete;
		};
	}
}

#endif
