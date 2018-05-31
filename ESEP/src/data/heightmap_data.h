#ifndef ESEP_DATA_HEIGHTMAP_H
#define ESEP_DATA_HEIGHTMAP_H

#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class HeightMap : public DataPoint
		{
			typedef uint16_t time_t;
			typedef uint16_t height_t;

			public:
				HeightMap() : DataPoint(DataPoint::Type::HEIGHT_MAP) { }
				void addHeightValue(time_t, height_t);

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				std::vector<std::pair<time_t, height_t>> mHeightValue;
		};
	}
}
#endif
