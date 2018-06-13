#include "master/plugin/hausdorff.h"

namespace esep { namespace master { namespace plugin {

float Hausdorff::match(const data_t& data)
{
	typedef data::DataPoint::Type Type;

	float r = 0.0;

	for(const auto& d : data)
	{
		if(d->type() == Type::HEIGHT_MAP)
		{
			r = 1.0 - use(dynamic_cast<impl::key_type>(d.get())).get<Station::HAUSDORFF>().distance(mProfile);
		}
	}

	return r < 0.0 ? 0.0 : r;
}

}}}
