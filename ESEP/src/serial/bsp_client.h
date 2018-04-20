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
			private:
			typedef std::unique_ptr<Connection> connection_ptr;

			public:
			static constexpr uint DEFAULT_TIMEOUT = 10;

			public:
				explicit BSPClient(connection_ptr, uint = DEFAULT_TIMEOUT);
				~BSPClient( );
				virtual void write(const buffer_t&);
				virtual buffer_t read( );
				size_t size( ) const;
			private:
				struct Impl;

				Impl *pImpl;
		};
	}
}

#endif
