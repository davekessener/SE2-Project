#ifndef ESEP_MASTER_HSM_SENDER_H
#define ESEP_MASTER_HSM_SENDER_H

#include "communication/IRecipient.h"
#include "communication/packet.h"

namespace esep
{
	namespace master
	{
		class Sender
		{
			typedef communication::Packet Packet;
			typedef Packet::Location Location;
			typedef Packet::msg_t msg_t;

			public:
				virtual ~Sender( ) { }
				virtual void send(Location, msg_t) = 0;
		};
	}
}

#endif
