#ifndef ESEP_BASE_DUMMYMASTER_H
#define ESEP_BASE_DUMMYMASTER_H

#include "communication/packet.h"
#include "communication/IRecipient.h"
#include "lib/utils.h"

namespace esep
{
	namespace base
	{
		class DummyMaster : public communication::IRecipient
		{
			public:
				void accept(communication::Packet_ptr p)
				{
					MXT_LOG(lib::stringify("Master received message ", static_cast<uint>(p->message())));
				}

				bool running( ) const { return true; }
		};
	}
}

#endif
