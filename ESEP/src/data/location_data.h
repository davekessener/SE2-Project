#ifndef ESEP_DATA_LOCATION_DATA_H
#define ESEP_DATA_LOCATION_DATA_H

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
					HAND_OVER,
					LB_START,
					LB_END,
					LB_HEIGHTSENSOR,
					LB_RAMP,
					LB_SWITCH
				};

				Location(Type v) : DataPoint(DataPoint::Type::LOCATION), mLocation(v) { }
				Type location( ) const { return mLocation; }

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				const Type mLocation;
		};
	}
}
#endif
