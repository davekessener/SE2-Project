#ifndef ESEP_BASE_ERROR_ANALYSE_H
#define ESEP_BASE_ERROR_ANALYSE_H

#include "base/IManager.h"
#include "base/error/reset_ack.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Analyse : public ResetAck
			{
				typedef communication::IRecipient IRecipient;
				typedef communication::Packet_ptr Packet_ptr;

				public:
					MXT_DEFINE_E(NoMessageInPacket);

				public:
					Analyse(IRecipient *, Packet_ptr);
					void enter() override;
					void handle(Event) override;
					void accept(Packet_ptr) override { };

				private:
					std::string mMessage;
			};
		}
	}
}

#endif
