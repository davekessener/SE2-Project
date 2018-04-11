#ifndef ESEP_SERIAL_ACTUAL_CONNECTION_H
#define ESEP_SERIAL_ACTUAL_CONNECTION_H

#include "serial/connection.h"

namespace esep
{
	namespace serial
	{
		class ActualConnection : public Connection
		{
			public:
				ActualConnection(const std::string&);
				~ActualConnection( );
				void write(const byte_t *, size_t);
				void read(byte_t *, size_t);
				void open(const std::string&);
				void close( );
			private:
				int fildes_;
		};
	}
}

#endif
