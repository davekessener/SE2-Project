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
			typedef communication::Message Message;
			typedef communication::IRecipient IRecipient;
			typedef communication::Packet Packet;
			typedef Packet::msg_t msg_t;
			typedef Packet::Location Location;

			public:
			MXT_DEFINE_E(UndefinedBaseException);

			public:
				DummyMaster(IRecipient *b) : mBase(b), mErrors(0) { }

				void accept(communication::Packet_ptr p)
				{
					if(!mBase)
					{
						MXT_THROW_EX(UndefinedBaseException);
					}

					MXT_LOG("Master received message ", lib::hex<16>(p->message()));

					if(p->message().is<Message::Master>())
						switch(p->message().as<Message::Master>())
					{
						case Message::Master::CONFIG:
							send(Location::BASE, Message::Base::CONFIG);
							break;
						case Message::Master::RUN:
							send(Location::BASE, Message::Base::RUN);
							break;
						case Message::Master::IDLE:
							send(Location::BASE, Message::Base::IDLE);
							break;
						case Message::Master::READY:
							send(Location::BASE, Message::Base::READY);
							break;
						case Message::Master::FIXED:
							if(!--mErrors)
							{
								send(Location::BASE, Message::Base::READY);
							}
							break;
					}
					else if(p->message().is<Message::Error>())
					{
						mErrors = 2;
						send(Location::BASE, p->message());
					}
					else if(p->message().is<Message::Config>())
						switch(p->message().as<Message::Config>())
					{
						case Message::Config::START:
							if(p->source() == Location::BASE_S)
							{
								send(Location::BASE_M, Message::Run::SUSPEND);
							}
							break;
						case Message::Config::DONE:
							if(p->source() == Location::BASE_M)
							{
								send(Location::BASE, Message::Run::RESUME);
							}
							else
							{
								send(Location::BASE, Message::Base::IDLE);
							}
							break;
						case Message::Config::FAILED:
							send(Location::BASE, Message::Error::CONFIG);
							break;
					}
				}

				bool running( ) const { return true; }

			private:
				void send(Location t, msg_t msg)
				{
					mBase->accept(std::make_shared<Packet>(Location::MASTER, t, msg));
				}

			private:
				IRecipient *mBase;
				uint mErrors;
		};
	}
}

#endif
