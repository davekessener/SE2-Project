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
				std::vector<uint16_t> vals;
				uint16_t min = 0xFFFF, max = 0;
				uint64_t mean = 0;

				for(const auto& v : static_cast<data::HeightMap&>(*d))
				{
					vals.push_back(v.second);

					if(min > v.second) min = v.second;
					if(max < v.second) max = v.second;

					mean += v.second;
				}

				std::sort(vals.begin(), vals.end());

				ss << "On M" << (i+1) << " measured {min = " << min
						<< ", max = " << max
						<< ", mean = " << (mean / vals.size())
						<< ", median = " << (vals.at(vals.size() / 2)) << "}\n";
			}
		}

		++i;
	}

	return ss.str();
}

}}
