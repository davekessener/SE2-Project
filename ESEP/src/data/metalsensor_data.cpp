#include "data/metalsensor_data.h"

#include "data/data_manager.h"

namespace esep { namespace data {

void MetalSensor::doSerialize(lib::ByteStream& bs) const
{
	bs.insert(mIsMetal ? 0xFF : 0x00);
}

Data_ptr MetalSensor::deserialize(lib::ByteStream& bs)
{
	if(bs.empty())
	{
		MXT_THROW_EX(DataManager::UnexpectedEOSException);
	}

	return Data_ptr(new MetalSensor(bs.remove() == 0xFF));
}

}}
