#ifndef SRC_COMMUNICATION_PACKET_H
#define SRC_COMMUNICATION_PACKET_H

#include <memory>

#include "lib/byte_stream.h"
#include "lib/enum.h"

#include "data/data_point.h"

#include "communication/message.h"

namespace esep
{
	namespace communication
	{
		class Packet
		{
			typedef std::vector<data::Data_ptr> container_t;
			typedef container_t::const_iterator iterator;

			public:
			MXT_DEFINE_E(NotImplementedException);

			enum class Location : uint8_t
			{
				MASTER,
				BASE,
				BASE_M,
				BASE_S
			};

			typedef tml::MakeCompoundEnum<
				Message::Master,
				Message::Base,
				Message::Config,
				Message::Run,
				Message::Error
			> msg_t;

			public :
				Packet(Location src, Location trg, msg_t msg);

				Location target() const { return mTarget; }
				Location source() const { return mSource; }
				msg_t message() const { return mMessage; }

				void target(Location v) { mTarget = v; }
				void source(Location v) { mSource = v; }
				void message(msg_t v) { mMessage = v; }

				void addDataPoint(std::shared_ptr<data::DataPoint>);
				iterator begin( ) const { return mDataPoints.cbegin(); }
				iterator end( ) const { return mDataPoints.cend(); }

				void serialize(lib::ByteStream&);

				static std::shared_ptr<Packet> deserialize(lib::ByteStream&);

			private :
				msg_t mMessage;
				Location mTarget;
				Location mSource;
				container_t mDataPoints;
		};

		typedef std::shared_ptr<Packet> Packet_ptr;
	}
}

#endif /* SRC_COMMUNICATION_PACKET_H_ */
