#ifndef ESEP_MASTER_HSM_MANAGER_H
#define ESEP_MASTER_HSM_MANAGER_H

#include "lib/hsm.h"

#include "communication/packet.h"

#include "master/hsm/base.h"

#include "master/sender.h"

namespace esep
{
	namespace master
	{
		namespace hsm
		{
			class Manager : public lib::hsm::Machine<Base>
			{
				typedef lib::hsm::State<Base> State;
				typedef lib::hsm::Machine<Base> Super;
				typedef communication::Message Message;
				typedef communication::Packet Packet;
				typedef communication::Packet_ptr Packet_ptr;
				typedef Packet::Location Location;
				typedef Packet::msg_t msg_t;

				public:
					Manager(State *p, Sender *r, Message::Base m) : Super(true, p), mCom(r), mMessage(m) { }
					void enter( ) override;

				protected:
					Sender& com( ) { return *mCom; }

				private:
					Sender * const mCom;
					Message::Base mMessage;
			};
		}
	}
}

#endif
