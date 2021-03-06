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

#include "master/plugin/hausdorff.h"

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
			typedef Plugin::Action Action;
			typedef data::Data_ptr Data_ptr;
			typedef plugin::Hausdorff::processor_t Processor;
			typedef std::function<void(const Item&)> item_handler_fn;

			public:
				Master(Processor *, IRecipient *, item_handler_fn);
				virtual ~Master( );

				void accept(Packet_ptr) override;

				void add(Plugin_ptr p) { mPlugins.push_back(p.release()); }

			private:
				void process(Item& item) override { mCallback(item); }
				void analyse(Item&, const data_t&) override;
				void evaluate(Item&, const history_t&) override;
				void message(Location, const std::string&);

			private:
				IRecipient * const mCom;
				Processor * const mProcessor;
				const item_handler_fn mCallback;
				std::vector<Plugin *> mPlugins;
				Logic mLogic;
		};
	}
}

#endif
