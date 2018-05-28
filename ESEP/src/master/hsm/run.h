#ifndef ESEP_MASTER_HSM_RUN_H
#define ESEP_MASTER_HSM_RUN_H

#include "lib/hsm.h"
#include "lib/petri_net.h"
#include "lib/segmented_buffer.h"

#include "master/hsm/base.h"
#include "master/hsm/manager.h"

#include "master/types.h"
#include "master/master.h"
#include "master/analyser.h"

namespace esep
{
	namespace master
	{
		namespace hsm
		{
			class Run : public Manager
			{
				typedef lib::hsm::State<Base> State;
				typedef communication::IRecipient IRecipient;
				typedef communication::Message Message;
				typedef communication::Packet Packet;
				typedef communication::Packet_ptr Packet_ptr;
				typedef lib::PetriNet<event_t> petri_t;
				typedef Packet::Location Location;
				typedef std::vector<Plugin::Type> history_t;

				static constexpr size_t NO_SEGMENTS = 10;

				public:
					Run(uint, State *, IRecipient *, Analyser *);

					void enter( ) override;
					void leave( ) override;

					void handle(Packet_ptr) override;

				private:
					history_t history(id_t);
					void flush( );

				private:
					Analyser * const mAnalyser;
					petri_t mLogic;
					lib::SegmentedBuffer<Item, NO_SEGMENTS> mItems;
					std::vector<Packet_ptr> mBuffer;
			};
		}
	}
}

#endif
