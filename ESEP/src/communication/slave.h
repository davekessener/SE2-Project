#ifndef ESEP_COMMUNICATION_SLAVE_H
#define ESEP_COMMUNICATION_SLAVE_H

#include <memory>

#include "serial/client.h"

#include "communication/base.h"

#define MXT_DEFAULT_SERIAL_DEVICE "/dev/ser4"

namespace esep
{
	namespace communication
	{
		class Slave : public Base
		{
			typedef std::unique_ptr<serial::Client> Client_ptr;

			public:
				Slave(Client_ptr);
				~Slave( );
				void accept(Packet_ptr) override;

			protected:
				void process(Packet_ptr) override;
		};
	}
}

#endif
