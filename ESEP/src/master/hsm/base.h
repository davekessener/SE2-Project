#ifndef ESEP_MASTER_HSM_BASE_H
#define ESEP_MASTER_HSM_BASE_H

#include "lib/utils.h"

#include "master/types.h"

#include "communication/packet.h"

namespace esep
{
	namespace master
	{
		namespace hsm
		{
			class Base
			{
				protected:
				typedef communication::Packet_ptr Packet_ptr;

				public:
				MXT_DEFINE_E(UnhandledEventException);

				public:
					virtual ~Base( ) { }
					virtual void handle(Packet_ptr);
			};
		}
	}
}

#endif
