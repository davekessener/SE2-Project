#include "data/data_manager.h"

#include "data/metalsensor_data.h"
#include "data/location_data.h"
#include "data/run_manager_timer_data.h"
#include "data/heightmap_data.h"

namespace esep { namespace data {

Data_ptr DataManager::deserialize(lib::ByteStream& bs)
{
	if(bs.empty())
	{
		MXT_THROW_EX(UnexpectedEOSException);
	}

	switch(bs.remove())
	{
	case static_cast<byte_t>(DataPoint::Type::METAL_SENSOR):
		return MetalSensor::deserialize(bs);

	case static_cast<byte_t>(DataPoint::Type::LOCATION):
		return Location::deserialize(bs);

	case static_cast<byte_t>(DataPoint::Type::RUN_MANAGER_TIMER):
		return RunManagerTimer::deserialize(bs);

	case static_cast<byte_t>(DataPoint::Type::HEIGHT_MAP):
		return HeightMap::deserialize(bs);

	default:
		MXT_THROW_EX(UnknownTypeException);
	}
}

}}
