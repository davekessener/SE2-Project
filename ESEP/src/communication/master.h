#ifndef ESEP_COMMUNICATION_MASTER_H
#define ESEP_COMMUNICATION_MASTER_H

#include <memory>

#include "serial/client.h"

#include "communication/base.h"

namespace esep
{
	namespace communication
	{
		class Master : public Base
		{
			typedef std::unique_ptr<serial::Client> Client_ptr;

			public:
				Master(IRecipient *, Client_ptr);
				~Master( );
				void accept(Packet_ptr) override;

			protected:
				void process(Packet_ptr) override;

			private:
				IRecipient * const mMaster;
		};
	}
}

#endif
