
#ifndef BASE_ERROR_ITEM_APPEARED_H
#define BASE_ERROR_ITEM_APPEARED_H

#include "base/error/reset_ack.h"
#include "data/location_data.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class ItemAppeared : public ResetAck
			{
				typedef communication::Packet_ptr Packet_ptr;
				typedef data::Location::Type Location;

				public:
					ItemAppeared(communication::IRecipient *, Location);
					void enter() override;
					void handle(Event) override;
					void accept(Packet_ptr) override { };

				private:
					Location mLocation;
			};
		}
	}
}


#endif
