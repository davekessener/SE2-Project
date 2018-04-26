/*
 * IRecipient.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_COMMUNICATION_IRECIPIENT_H_
#define SRC_COMMUNICATION_IRECIPIENT_H_

namespace esep
{
	namespace communication
	{
		class IRecipient
		{
			public :
				IRecipient();
				virtual ~IRecipient();

				virtual void accept(Message);

		};
}
}



#endif /* SRC_COMMUNICATION_IRECIPIENT_H_ */
