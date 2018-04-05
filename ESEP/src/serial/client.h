#ifndef ESEP_SERIAL_CLIENT_H
#define ESEP_SERIAL_CLIENT_H

#include <vector>

#include "lib/utils.h"
#include "serial/connection.h"

namespace esep
{
	namespace serial
	{
		class Client
		{
			public:
			typedef std::vector<byte_t> buffer_t;

			public:
				Client(Connection&&);
				void write(const buffer_t&);
				buffer_t read( );
			private:
		};
	}
}

#endif
