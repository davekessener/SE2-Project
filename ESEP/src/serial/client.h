#ifndef ESEP_SERIAL_CLIENT_H
#define ESEP_SERIAL_CLIENT_H

#include <vector>

#include "lib/utils.h"
#include "lib/byte_buffer.h"
#include "serial/connection.h"

namespace esep
{
	namespace serial
	{
		class Client
		{
			public:
			typedef lib::ByteBuffer<> buffer_t;

			public:
				Client(Connection&&);
				~Client( );
				void write(const buffer_t&);
				buffer_t read( );
			private:
				struct Impl;

				Impl *pImpl;
		};
	}
}

#endif
