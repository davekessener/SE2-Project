#ifndef ESEP_DATA_MESSAGE_H
#define ESEP_DATA_MESSAGE_H

#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class Message : public DataPoint
		{
			public:
				Message(const std::string& msg) : DataPoint(Type::MESSAGE), mMsg(msg) { }

				const std::string& message( ) const { return mMsg; }

				std::string to_s( ) const override { return lib::stringify("%[", mMsg, "]"); }

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				std::string mMsg;
		};
	}
}

#endif
