#ifndef ESEP_SERIAL_DUMMY_CONNECTION_H
#define ESEP_SERIAL_DUMMY_CONNECTION_H

#include <functional>

#include "serial/connection.h"

namespace esep
{
	namespace serial
	{
		class DummyConnection : public Connection
		{
			public:
			typedef std::function<void(byte_t)> write_fn;
			typedef std::function<byte_t(void)> read_fn;

			public:
				DummyConnection(read_fn rf, write_fn wf)
					: mRead(rf), mWrite(wf) { }
				void write(const byte_t *, size_t);
				void read(byte_t *, size_t);
				void open(const std::string&) { }
				void close( ) { }
			private:
				read_fn mRead;
				write_fn mWrite;
		};
	}
}

#endif
