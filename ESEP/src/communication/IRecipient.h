/*
 * IRecipient.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_COMMUNICATION_IRECIPIENT_H_
#define SRC_COMMUNICATION_IRECIPIENT_H_

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

				virtual void accept(std::shared_ptr<Packet>) = 0;


		};
}
}



#endif /* SRC_COMMUNICATION_IRECIPIENT_H_ */
