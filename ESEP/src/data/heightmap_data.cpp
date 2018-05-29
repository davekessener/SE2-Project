#include "data/heightmap_data.h"

#include "data/data_manager.h"

namespace esep { namespace data {

void HeightMap::doSerialize(lib::ByteStream& bs) const
{
}

Data_ptr HeightMap::deserialize(lib::ByteStream& bs)
{
	MXT_THROW_EX(DataManager::UnexpectedEOSException);
}

}}


