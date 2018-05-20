#ifndef ESEP_MASTER_H
#define ESEP_MASTER_H

#include "communication/IRecipient.h"
#include "communication/packet.h"

#include "lib/utils.h"
#include "lib/hsm.h"

#include "master/types.h"
#include "master/plugin.h"

namespace esep
{
	namespace master
	{
		class Master : public communication::IRecipient
		{
			typedef communication::IRecipient IRecipient;
			typedef communication::Message Message;
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef hsm::Hierarchy<event_t> hsm_t;
			typedef std::pair<id_t, Plugin *> item_t;

			public:
				Master(IRecipient *);
				virtual ~Master( );

				void accept(Packet_ptr) override;

			private:
				IRecipient * const mCom;
				std::vector<Plugin *> mPlugins;
				std::deque<item_t> mItems;
				hsm_t mLogic;
		};
	}
}

#endif
