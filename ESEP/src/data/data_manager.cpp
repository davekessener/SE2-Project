#include "data/data_manager.h"

#include "data/metalsensor_data.h"

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
		break;
	default:
		MXT_THROW_EX(UnknownTypeException);
	}
}

}}
