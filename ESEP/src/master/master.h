#ifndef ESEP_MASTER_H
#define ESEP_MASTER_H

#include "communication/IRecipient.h"
#include "communication/packet.h"

#include "data/data_point.h"

#include "lib/utils.h"
#include "lib/hsm.h"

#include "master/types.h"
#include "master/plugin.h"
#include "master/analyser.h"
#include "master/logic.h"

#include "master/hsm/base.h"

namespace esep
{
	namespace master
	{
		class Master : private Analyser, public communication::IRecipient
		{
			typedef communication::IRecipient IRecipient;
			typedef communication::Message Message;
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef Packet::Location Location;
			typedef Packet::msg_t msg_t;
			typedef data::Data_ptr Data_ptr;
			typedef std::function<void(const Item&)> item_handler_fn;

			public:
				Master(IRecipient *, item_handler_fn);
				virtual ~Master( );

				void accept(Packet_ptr) override;

				void add(Plugin_ptr p) { mPlugins.push_back(p.release()); }

			private:
				void analyse(Item&, const data_t&) override;
				void evaluate(Item&, const history_t&) override;
				void process(Item& item) override { mCallback(item); }

			private:
				IRecipient * const mCom;
				const item_handler_fn mCallback;
				std::vector<Plugin *> mPlugins;
				Logic mLogic;
		};
	}
}

#endif
