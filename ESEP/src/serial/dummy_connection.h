#ifndef ESEP_SERIAL_DUMMY_CONNECTION_H
#define ESEP_SERIAL_DUMMY_CONNECTION_H

#include <functional>

#include "serial/connection.h"

#include "lib/sync/container.h"

namespace esep
{
	namespace serial
	{
		class DummyConnection : public Connection
		{
			public:
			typedef sync::Container<byte_t> buffer_t;

			public:
				DummyConnection( ) : mCounterpart(nullptr) { }
				void connect(DummyConnection& c) { c.close(); mCounterpart = &c; c.mCounterpart = this; }
				void write(const byte_t *, size_t);
				void read(byte_t *, size_t);
				void open(const std::string&) { MXT_THROW("Cannot open a dummy-connection!"); }
				void close( ) { mCounterpart = nullptr; mBuffer.clear(); }
				bool isOpen( ) const { return mCounterpart; }
			private:
				DummyConnection *mCounterpart;
				buffer_t mBuffer;
		};
	}
}

#endif
