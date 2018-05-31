#ifndef ESEP_MASTER_HSM_MANAGER_H
#define ESEP_MASTER_HSM_MANAGER_H

#include "lib/hsm.h"

#include "communication/IRecipient.h"
#include "communication/packet.h"

#include "master/hsm/base.h"

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
				typedef communication::IRecipient IRecipient;
				typedef communication::Message Message;
				typedef communication::Packet Packet;
				typedef communication::Packet_ptr Packet_ptr;
				typedef Packet::Location Location;
				typedef Packet::msg_t msg_t;

				public:
					Manager(State *p, IRecipient *r, Message::Base m) : Super(true, p), mCom(r), mMessage(m) { }
					void enter( ) override;

				protected:
					IRecipient& com( ) { return *mCom; }
					void send(Location l, msg_t msg) { mCom->accept(std::make_shared<Packet>(Location::MASTER, l, msg)); }

				private:
					IRecipient * const mCom;
					Message::Base mMessage;
			};
		}
	}
}

#endif
