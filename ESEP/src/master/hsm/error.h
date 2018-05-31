#ifndef ESEP_MASTER_HSM_ERROR_H
#define ESEP_MASTER_HSM_ERROR_H

#include "lib/hsm.h"

#include "communication/IRecipient.h"

#include "master/hsm/base.h"

namespace esep
{
	namespace master
	{
		namespace hsm
		{
			class Error : public lib::hsm::Leaf<Base>
			{
				typedef lib::hsm::State<Base> State;
				typedef lib::hsm::Leaf<Base> Super;
				typedef communication::IRecipient IRecipient;
				typedef communication::Packet Packet;
				typedef communication::Packet_ptr Packet_ptr;
				typedef communication::Message Message;
				typedef Packet::Location Location;

				public:
					Error(State *p, IRecipient *r) : Super(p), mCom(r) { }

					void handle(Packet_ptr) override;

				private:
					IRecipient * const mCom;
			};
		}
	}
}

#endif
