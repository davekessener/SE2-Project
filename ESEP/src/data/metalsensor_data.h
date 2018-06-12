#ifndef ESEP_DATA_METALSENSOR_H
#define ESEP_DATA_METALSENSOR_H

#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class MetalSensor : public DataPoint
		{
			public:
				MetalSensor(bool v) : DataPoint(Type::METAL_SENSOR), mIsMetal(v) { }
				bool isMetal( ) const { return mIsMetal; }

				std::string to_s( ) const override { return mIsMetal ? "METAL" : "PLASTIC"; }

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				const bool mIsMetal;
		};
	}
}

#endif
