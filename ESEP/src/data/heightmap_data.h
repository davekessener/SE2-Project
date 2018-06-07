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
			typedef uint16_t time_t;
			typedef uint16_t height_t;
			typedef std::pair<time_t, height_t> value_type;
			typedef std::vector<value_type> container_type;
			typedef container_type::const_iterator iterator;

			public:
				HeightMap() : DataPoint(DataPoint::Type::HEIGHT_MAP) { }
				void addHeightValue(time_t, height_t);

				iterator begin( ) const { return mHeightValues.begin(); }
				iterator end( ) const { return mHeightValues.end(); }

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				container_type mHeightValues;
		};
	}
}
#endif
