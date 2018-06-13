
#ifndef BASE_ERROR_ITEM_DISAPPEARED_H
#define BASE_ERROR_ITEM_DISAPPEARED_H

#include "base/error/reset_ack.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class ItemDisappeared : public ResetAck
			{
				typedef communication::Packet_ptr Packet_ptr;

				public:
					ItemDisappeared(communication::IRecipient *);
					void enter() override;
					void handle(Event) override;
					void accept(Packet_ptr) override { };

			};
		}
	}
}

#endif
