/*
 * message.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_COMMUNICATION_PACKET_H_
#define SRC_COMMUNICATION_PACKET_H_

#include <memory>

#include "lib/byte_stream.h"
#include "data/data_point.h"



namespace esep
{
	namespace communication
	{

		class Packet
		{

			public:
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

			public :
				Packet(Location src, Location trg, Message msg);
				virtual ~Packet();


				Location target();
				Location source();
				Message message();
				//void addDataPoint(data::DataPoint*);
				void addDataPoint(std::shared_ptr<data::DataPoint>);
				void serialize(lib::ByteStream&);
				static std::shared_ptr<Packet> deserialize(lib::ByteStream&);


			private :
				Message mMessage;
				Location mTarget;
				Location mSource;
				std::vector<std::shared_ptr<data::DataPoint>> mDataPoints;

		};
}
}


#endif /* SRC_COMMUNICATION_PACKET_H_ */
