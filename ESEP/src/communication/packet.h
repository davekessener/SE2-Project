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
			typedef std::vector<data::Data_ptr> container_t;
			typedef container_t::const_iterator iterator;

			public:
			MXT_DEFINE_E(NotImplException);

			enum class Message : int8_t
			{
				SELECT_CONFIG,
				SELECT_RUN,
				IDLE,
				RESUME,
				SUSPEND,
				KEEP_NEXT,
				ANALYSE,
				CONFIG_DONE,
				CONFIG_FAILED,
				ITEM_APPEARED,
				ITEM_DISAPPEARED,
				ERROR_SERIAL,
				NEW_ITEM,
				REACHED_END,
				RAMP_FULL,
				ERROR_FIXED,
				ESTOP,
				WARNING,
				SHUTDOWN
			};

			enum class Config
			{
				START,
				DONE,
				FAILED
			};

			enum class Error
			{
				CONFIG,
				SERIAL,
				ITEM_APPEARED,  // (Base -> Master)
				ITEM_DISAPPEARED,  // (Base -> Master)
				ESTOP,
				RAMP_FULL,
				WARNING,
				FIXED
			};

			enum class Run
			{
				NEW_ITEM,
				REACHED_END,
				RAMP_FULL,
				ANALYSE,  // analyse item at switch (Base -> Master)
				RESUME, // start motor (Master -> Base)
				SUSPEND, // stop motor (Master -> Base)
				KEEP_NEXT  // open switch (Master -> Base)
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

				void target(Location v) { mTarget = v; }
				void source(Location v) { mSource = v; }
				void message(Message v) { mMessage = v; }

				void addDataPoint(std::shared_ptr<data::DataPoint>);
				iterator begin( ) const { return mDataPoints.cbegin(); }
				iterator end( ) const { return mDataPoints.cend(); }

				void serialize(lib::ByteStream&);

				static std::shared_ptr<Packet> deserialize(lib::ByteStream&);

			private :
				Message mMessage;
				Location mTarget;
				Location mSource;
				container_t mDataPoints;
		};

		typedef std::shared_ptr<Packet> Packet_ptr;
	}
}

#endif /* SRC_COMMUNICATION_PACKET_H_ */
