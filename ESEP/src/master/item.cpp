#include "master.h"

#include "communication/packet.h"

#include "master/plugin.h"

#include "lib/timer.h"

namespace esep { namespace master {

id_t Item::NEXT_ID = 0;

Item::Item(void)
	: mID(NEXT_ID++)
	, mTime(0)
	, mAction(Plugin::Action::VOID)
	, mPlugin(nullptr)
	, mLocation(Location::BASE)
{
}

std::string Item::to_s(void) const
{
	std::stringstream ss;
	uint i = 0;

	ss << "ITEM #" << ID() << " [" << master::Plugin::type_to_s(plugin()->type()) << "] @" << lib::Timer::instance().elapsed() << "ms\n";

	for(const auto& data : mData)
	{
		for(const auto& d : data)
		{
			if(d->type() == data::DataPoint::Type::HEIGHT_MAP)
			{
				uint16_t min = 0xFFFF, max = 0;

				for(const auto& p : static_cast<data::HeightMap&>(*d))
				{
					if(min > p.second) min = p.second;
					if(max < p.second) max = p.second;
				}

				ss << "On M" << (i+1) << " measured {min = " << min << ", max = " << max << ", mean = " << (min / 2 + max / 2) << "}\n";
			}
		}

		++i;
	}

	return ss.str();
}

}}
