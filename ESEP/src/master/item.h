#ifndef ESEP_MASTER_ITEM_H
#define ESEP_MASTER_ITEM_H

#include <vector>

#include "communication/packet.h"

#include "data/data_point.h"

#include "lib/utils.h"

#include "master/plugin.h"

namespace esep
{
	namespace master
	{
		class Item
		{
			typedef data::Data_ptr Data_ptr;
			typedef communication::Packet Packet;
			typedef Packet::Location Location;
			typedef std::vector<Data_ptr> data_t;
			typedef std::vector<data_t> data_vec_t;

			public:
				Item( );
				id_t ID( ) const { return mID; }
				uint64_t timestamp( ) const { return mTime; }
				Plugin::Action action( ) const { return mAction; }
				Plugin *plugin( ) const { return mPlugin; }
				Location location( ) const { return mLocation; }
				data_vec_t& data( ) { return mData; }

				void timestamp(uint64_t v) { mTime = v; }
				void action(Plugin::Action a) { if(static_cast<uint>(mAction) < static_cast<uint>(a)) mAction = a; }
				void plugin(Plugin *p) { mPlugin = p; }
				void location(Location l) { mLocation = l; }

			private:
				const id_t mID;
				uint64_t mTime;
				Plugin::Action mAction;
				Plugin *mPlugin;
				data_vec_t mData;
				Location mLocation;

				static id_t NEXT_ID;
		};
	}
}

#endif
