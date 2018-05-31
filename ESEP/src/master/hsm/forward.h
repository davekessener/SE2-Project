#ifndef ESEP_MASTER_HSM_FORWARD_H
#define ESEP_MASTER_HSM_FORWARD_H

#include "lib/hsm.h"

#include "communication/packet.h"

#include "master/hsm/base.h"

namespace esep
{
	namespace master
	{
		namespace hsm
		{
			class Forward : public lib::hsm::Leaf<Base>
			{
				typedef lib::hsm::Leaf<Base> Super;
				typedef lib::hsm::State<Base> State;

				public:
					Forward(State *p, Base *f) : Super(p), mForward(f) { }

					void handle(Packet_ptr p) override { mForward->handle(p); }

				private:
					Base * const mForward;
			};
		}
	}
}

#endif
