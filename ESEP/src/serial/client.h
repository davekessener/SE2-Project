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

			public:
				virtual ~Client( ) { }
				virtual void write(const buffer_t&) = 0;
				virtual buffer_t read( ) = 0;
				virtual bool connected( ) const = 0;
			private:
		};
	}
}

#endif
