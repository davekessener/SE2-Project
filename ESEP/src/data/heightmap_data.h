#ifndef ESEP_DATA_LOCATION_H
#define ESEP_DATA_LOCATION_H

#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class HeightMap : public DataPoint
		{

			public:

				Location(Type v) : DataPoint(DataPoint::Type::LOCATION), mLocation(v) { }
				Type location( ) const { return mLocation; }

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				const ;
		};
	}
}
#endif
