#include <data/location_data.h>
#include "data/data_manager.h"

namespace esep { namespace data {

namespace
{
	std::string location_type_to_s(Location::Type t)
	{
		switch(t)
		{
		case Location::Type::LB_START:
			return "LB_START";

		case Location::Type::LB_END:
			return "LB_END";

		case Location::Type::LB_HEIGHTSENSOR:
			return "LB_HEIGHTSENSOR";

		case Location::Type::LB_RAMP:
			return "LB_RAMP";

		case Location::Type::LB_SWITCH:
			return "LB_SWITCH";
		}

		return "ERROR";
	}
}

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

std::string Location::to_s(void) const
{
	return lib::stringify("@", location_type_to_s(location()));
}

}}
