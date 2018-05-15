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
			typedef communication::IRecipient IRecipient;
			typedef communication::Packet Packet;
			typedef Packet::Location Location;
			typedef Packet::Message Message;

			public:
			MXT_DEFINE_E(UndefinedBaseException);

			public:
				DummyMaster(IRecipient *b) : mBase(b) { }

				void accept(communication::Packet_ptr p)
				{
					if(!mBase)
					{
						MXT_THROW_EX(UndefinedBaseException);
					}

					MXT_LOG("Master received message ", static_cast<uint>(p->message()));

					switch(p->message())
					{
					case Message::SELECT_CONFIG:
						send(Location::BASE, Message::SELECT_CONFIG);
						break;
					case Message::SELECT_RUN:
						send(Location::BASE, Message::SHUTDOWN);
						break;
					case Message::CONFIG_DONE:
						if(p->source() == Location::BASE_S)
						{
							send(Location::BASE, Message::IDLE);
						}
						else
						{
							send(Location::BASE, Message::RESUME);
						}
						break;
					case Message::ESTOP:
						send(Location::BASE, Message::SHUTDOWN);
						break;
					default:
						break;
					}
				}

				bool running( ) const { return true; }

			private:
				void send(Location t, Message msg)
				{
					mBase->accept(std::make_shared<Packet>(Location::MASTER, t, msg));
				}

			private:
				IRecipient *mBase;
		};
	}
}

#endif
