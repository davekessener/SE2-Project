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
			private:
				int fildes_;
			public:
				Connection(const std::string&);
				Connection(Connection&&);
				~Connection( );
				void write(const byte_t *, size_t);
				void read(byte_t *, size_t);
			private:
				Connection(const Connection&) = delete;
				Connection& operator=(const Connection&) = delete;
		};
	}
}

#endif
