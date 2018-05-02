/*
 * IRecipient.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_COMMUNICATION_IRECIPIENT_H
#define SRC_COMMUNICATION_IRECIPIENT_H

#include <memory>

#include "communication/packet.h"

namespace esep
{
	namespace communication
	{
		class IRecipient
		{
			public:
				virtual ~IRecipient() { }
				virtual void accept(Packet_ptr) = 0;
		};
	}
}

#endif /* SRC_COMMUNICATION_IRECIPIENT_H_ */
