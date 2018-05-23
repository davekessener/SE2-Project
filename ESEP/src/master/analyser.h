#ifndef ESEP_MASTER_ANALYSER_H
#define ESEP_MASTER_ANALYSER_H

#include <vector>

#include "data/data_point.h"

#include "communication/packet.h"

#include "master/item.h"

namespace esep
{
	namespace master
	{
		class Analyser
		{
			public:
			typedef data::Data_ptr Data_ptr;
			typedef std::vector<Data_ptr> data_t;
			typedef std::vector<Plugin::Type> history_t;
			typedef communication::Packet Packet;
			typedef Packet::Location Location;

			public:
				virtual ~Analyser( ) { }

				virtual void analyse(Item&, const data_t&) = 0;
				virtual void evaluate(Item&, const history_t&) = 0;
		};
	}
}

#endif
