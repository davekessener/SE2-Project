#ifndef ESEP_MASTER_HSM_ERROR_H
#define ESEP_MASTER_HSM_ERROR_H

#include "lib/hsm.h"

#include "master/hsm/base.h"

#include "master/sender.h"

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
				typedef communication::Packet Packet;
				typedef communication::Packet_ptr Packet_ptr;
				typedef Packet::Location Location;

				public:
					Error(State *p, Sender *r) : Super(p), mCom(r) { }

					void handle(Packet_ptr) override;

				private:
					Sender * const mCom;
			};
		}
	}
}

#endif
