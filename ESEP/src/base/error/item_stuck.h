
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
				typedef data::DataPoint::Type DataType;
				typedef data::Location Location;

				public:
					MXT_DEFINE_E(NoLocationInPacket);

				public:
					ItemStuck(communication::IRecipient *, Packet_ptr);
					void enter() override;
					void handle(Event) override;
					void accept(Packet_ptr) override { };

				private:
					void tryResolve( );

				private:
					std::shared_ptr<Location> mLocation;
			};
		}
	}
}

#endif
