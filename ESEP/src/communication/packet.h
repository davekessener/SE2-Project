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

		enum class Message : int8_t
		{
			START_CONFIG,
			START_RUN
		};

		enum class Location : int8_t
		{
			BASE_M,
			BASE_S,
			MASTER,
			BASE
		};

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

			private :
				Message mMessage;
				Location mTarget;
				Location mSource;



		};
}
}



#endif /* SRC_COMMUNICATION_PACKET_H_ */
