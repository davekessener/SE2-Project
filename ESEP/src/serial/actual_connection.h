#ifndef ESEP_SERIAL_ACTUAL_CONNECTION_H
#define ESEP_SERIAL_ACTUAL_CONNECTION_H

#include <string>

#include "lib/utils.h"
#include "serial/connection.h"

namespace esep
{
	namespace serial
	{
		class ActualConnection : public Connection
		{
			public:
			static constexpr int INVALID_FD = -1;

			public:
				ActualConnection(const std::string&);
				virtual ~ActualConnection( );
				virtual void write(const byte_t *, size_t);
				virtual void read(byte_t *, size_t);
				virtual void open(const std::string&);
				virtual void close( );
				virtual bool isOpen( ) const;

			private:
				int mFD;

			private:
				ActualConnection(const ActualConnection&) = delete;
				ActualConnection& operator=(const ActualConnection&) = delete;
		};
	}
}

#endif
