
#ifndef BASE_ERROR_ITEM_STUCK_H
#define BASE_ERROR_ITEM_STUCK_H

#include "base/error/reset_ack.h"
#include "data/location_data.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class ItemStuck : public ResetAck
			{
				typedef communication::Packet_ptr Packet_ptr;
				typedef data::Location::Type Location;

				public:
					ItemStuck(communication::IRecipient *, Location);
					void handle(Event) override;
					void accept(Packet_ptr) override { };

				private:
					Location mLocation;
			};
		}
	}
}

#endif
