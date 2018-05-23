#ifndef ESEP_MASTER_H
#define ESEP_MASTER_H

#include "communication/IRecipient.h"
#include "communication/packet.h"

#include "lib/utils.h"
#include "lib/hsm.h"

#include "master/types.h"
#include "master/sender.h"
#include "master/plugin.h"
#include "master/analyser.h"
#include "master/logic.h"

#include "master/hsm/base.h"

namespace esep
{
	namespace master
	{
		class Master : private Sender, private Analyser, public communication::IRecipient
		{
			typedef communication::IRecipient IRecipient;
			typedef communication::Message Message;
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef Packet::Location Location;
			typedef Packet::msg_t msg_t;

			public:
				Master(IRecipient *);
				virtual ~Master( );

				void accept(Packet_ptr) override;
				void send(Location, msg_t) override;

			private:
				void analyse(Item&, const data_t&) override;
				void evaluate(Item&, const history_t&) override;

			private:
				IRecipient * const mCom;
				std::vector<Plugin *> mPlugins;
				Logic mLogic;
		};
	}
}

#endif
