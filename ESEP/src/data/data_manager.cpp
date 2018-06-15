#include "data/data_manager.h"

#include "data/metalsensor_data.h"
#include "data/location_data.h"
#include "data/run_manager_timer_data.h"
#include "data/heightmap_data.h"
#include "data/message_data.h"

namespace esep { namespace data {

Data_ptr DataManager::deserialize(lib::ByteStream& bs)
{
	if(bs.empty())
	{
		MXT_THROW_EX(UnexpectedEOSException);
	}

	switch(static_cast<DataPoint::Type>(bs.remove()))
	{
	case DataPoint::Type::METAL_SENSOR:
		return MetalSensor::deserialize(bs);

	case DataPoint::Type::LOCATION:
		return Location::deserialize(bs);

	case DataPoint::Type::RUN_MANAGER_TIMER:
		return RunManagerTimer::deserialize(bs);

	case DataPoint::Type::HEIGHT_MAP:
		return HeightMap::deserialize(bs);

	case DataPoint::Type::MESSAGE:
		return Message::deserialize(bs);
	}

	MXT_THROW_EX(UnknownTypeException);
}

}}
