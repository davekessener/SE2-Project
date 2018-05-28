#include <data/location_data.h>
#include "data/data_manager.h"

namespace esep { namespace data {

void Location::doSerialize(lib::ByteStream& bs) const
{
	bs << mLocation;
}

Data_ptr Location::deserialize(lib::ByteStream& bs)
{
	if(bs.empty())
	{
		MXT_THROW_EX(DataManager::UnexpectedEOSException);
	}

	return Data_ptr(new Location(static_cast<Type> (bs.remove())));
}

}}


