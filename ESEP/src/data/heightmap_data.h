#ifndef ESEP_DATA_HEIGHTMAP_H
#define ESEP_DATA_HEIGHTMAP_H

#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class HeightMap : public DataPoint
		{
			public:
				HeightMap(Type v) : DataPoint(DataPoint::Type::HEIGHT_MAP) { }

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
		};
	}
}
#endif
