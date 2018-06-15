#ifndef ESEP_DATA_LOCATION_H
#define ESEP_DATA_LOCATION_H

#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class Location : public DataPoint
		{
			public:
				enum class Type : byte_t
				{
					LB_START,
					LB_HEIGHTSENSOR,
					LB_SWITCH,
					LB_END,
					LB_RAMP,
				};

				Location(Type v) : DataPoint(DataPoint::Type::LOCATION), mLocation(v) { }
				Type location( ) const { return mLocation; }

				std::string to_s( ) const override;

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				const Type mLocation;
		};
	}
}
#endif
