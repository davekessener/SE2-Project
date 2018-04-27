/*
 * message.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_COMMUNICATION_PACKET_H_
#define SRC_COMMUNICATION_PACKET_H_

#include "lib/byte_stream.h"


namespace esep
{
	namespace communication
	{

		class Packet
		{
			public :
				Packet();
				virtual ~Packet();

				Location target();
				Location source();
				Message message();
				void serialize(ByteStream &);
				static void deserialize();

				enum class Message : int8_t
						{
							START_CONFIG,
							START_RUN,
							IDLE,
							RESUME,
							SUSPEND,
							KEEP_NExt,
							ANALYSE,
							CONFIG_DONE,
							SERIAL_ERROR,
							ITEM_APPEARED,
							ITEM_DISAPPEARED
						};

				enum class Location : int8_t
						{
							BASE_M,
							BASE_S,
							MASTER,
							BASE
						};

			private :
				Message mMessage;
				Location mTarget;
				Location mSource;



		};
}
}



#endif /* SRC_COMMUNICATION_PACKET_H_ */
