/*
 * IRecipient.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_COMMUNICATION_IRECIPIENT_H
#define SRC_COMMUNICATION_IRECIPIENT_H

#include <memory>
#include "data/packet.h"
namespace esep
{
	namespace communication
	{
		class IRecipient
		{
			public:
				IRecipient();
				virtual ~IRecipient()
				{
					delete this();
				}

				virtual void accept(std::shared_ptr<Packet>) = 0;


		};
}
}



#endif /* SRC_COMMUNICATION_IRECIPIENT_H_ */
