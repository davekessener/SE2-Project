#ifndef ESEP_SERIAL_CLIENT_PACKET_BASE_H
#define ESEP_SERIAL_CLIENT_PACKET_BASE_H

#include <memory>

#include "serial/client/types.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			class Base
			{
				public:
					virtual ~Base( ) { }
					virtual types::buffer_t serialize( ) = 0;
				private:
			};

			typedef std::shared_ptr<Base> packet_ptr;
		}
	}
}

#endif
