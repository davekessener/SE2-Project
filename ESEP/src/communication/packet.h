/*
 * message.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_COMMUNICATION_PACKET_H
#define SRC_COMMUNICATION_PACKET_H

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
			MXT_DEFINE_E(NotImplException);

			enum class Message : int8_t
			{
				SELECT_CONFIG,
				SELECT_RUN,
				IDLE,
				RESUME, // start motor (Master -> Base)
				SUSPEND, // stop motor (Master -> Base)
				KEEP_NEXT, // open switch (Master -> Base)
				ANALYSE, // analyse item at switch (Base -> Master)
				CONFIG_DONE,
				CONFIG_FAILED,
				ITEM_APPEARED, // (Base -> Master)
				ITEM_DISAPPEARED // (Base -> Master)
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

				Location target() const { return mTarget; }
				Location source() const { return mSource; }
				Message message() const { return mMessage; }

				void addDataPoint(std::shared_ptr<data::DataPoint>);
				void serialize(lib::ByteStream&);

				static std::shared_ptr<Packet> deserialize(lib::ByteStream&);

			private :
				const Message mMessage;
				const Location mTarget;
				const Location mSource;
				std::vector<std::shared_ptr<data::DataPoint>> mDataPoints;
		};

		typedef std::shared_ptr<Packet> Packet_ptr;
	}
}

#endif /* SRC_COMMUNICATION_PACKET_H_ */
