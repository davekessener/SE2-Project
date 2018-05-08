#include <forward_list>
#include <algorithm>

#include "emp/location.h"

#include "lib/logger.h"

namespace esep { namespace emp {

typedef std::unique_lock<std::mutex> lock_t;

Location Location::byEvent(Event e)
{
	uint64_t v(static_cast<uint64_t>(e));

	return findLocation([&v](const Location& l) {
		return static_cast<uint>(l.field) == (v >> 32) && l.pin == (v & 0xFFFFFFFF);
	});
}

Location Location::findLocation(predicate_fn f)
{
	static std::forward_list<Location> locations;
	static std::mutex mtx;

	if(locations.empty())
	{
		lock_t lock(mtx);

		if(locations.empty())
		{
			// TODO
		}
	}

	auto i = std::find_if(locations.begin(), locations.end(), f);

	if(i == locations.end())
	{
		MXT_THROW_EX(UnknownLocationException);
	}
	else
	{
		return *i;
	}
}

}}
