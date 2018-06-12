#ifndef SRC_DATA_DATA_POINT_H
#define SRC_DATA_DATA_POINT_H

#include <memory>

#include "lib/byte_stream.h"

namespace esep
{
	namespace data
	{
		class DataPoint
		{
			public:
			enum class Type : byte_t
			{
				METAL_SENSOR,
				LOCATION,
				RUN_MANAGER_TIMER,
				HEIGHT_MAP
			};

			public:
				DataPoint(Type t) : mType(t) { }
				virtual ~DataPoint() {}
				void serialize(lib::ByteStream&) const;
				Type type( ) const { return mType; }

				virtual std::string to_s( ) const = 0;

			protected:
				virtual void doSerialize(lib::ByteStream&) const = 0;

			private:
				const Type mType;
		};

		typedef std::shared_ptr<DataPoint> Data_ptr;
	}
}

inline std::ostream& operator<<(std::ostream& os, const esep::data::Data_ptr& p)
{
	return os << p->to_s();
}

#endif /* SRC_DATA_DATA_POINT_H */
