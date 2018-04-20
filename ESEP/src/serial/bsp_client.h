#ifndef ESEP_SERIAL_BSPCLIENT_H
#define ESEP_SERIAL_BSPCLIENT_H

#include <memory>

#include "serial/client.h"

namespace esep
{
	namespace serial
	{
		class BSPClient : public Client
		{
			typedef std::unique_ptr<Connection> connection_ptr;

			public:
				explicit BSPClient(connection_ptr);
				~BSPClient( );
				virtual void write(const buffer_t&);
				virtual buffer_t read( );
			private:
				struct Impl;

				Impl *pImpl;
		};
	}
}

#endif
