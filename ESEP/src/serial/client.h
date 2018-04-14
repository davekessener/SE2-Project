#ifndef ESEP_SERIAL_CLIENT_H
#define ESEP_SERIAL_CLIENT_H

#include <vector>

#include "serial/connection.h"
#include "lib/utils.h"

namespace esep
{
	namespace serial
	{
		class Client
		{
			public:
			typedef std::vector<byte_t> buffer_t;
			typedef buffer_t value_type;
			typedef Connection::ConnectionClosedException ClosingException;

			public:
				explicit Client(Connection&);
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
