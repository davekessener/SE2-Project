#ifndef ESEP_MASTER_LOGIC_H
#define ESEP_MASTER_LOGIC_H

#include "lib/utils.h"
#include "lib/hsm.h"

#include "communication/packet.h"

#include "master/types.h"
#include "master/sender.h"
#include "master/analyser.h"

#include "master/hsm/base.h"

namespace esep
{
	namespace master
	{
		class Logic
		{
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef Packet::Location Location;
			typedef lib::hsm::Hierarchy<event_t, hsm::Base> hsm_t;

			public:
				Logic(Sender *, Analyser *);
				void process(event_t);
				void accept(Packet_ptr);

			private:
				hsm_t mLogic;
		};
	}
}

#endif
