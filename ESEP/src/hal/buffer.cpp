#include "hal/buffer.h"

namespace esep { namespace hal {

void Buffer::update(Field f, uint32_t v)
{
	auto& r(mFields[static_cast<uint>(f)]);
	auto c = r ^ v;

	r = v;

	if(f == Field::GPIO_0)
	{
		for(auto i1 = HAL::EVENTS, i2 = i1 + HAL::N_EVENTS ; i1 != i2 ; ++i1)
		{
			if(c & (1 << static_cast<uint>(*i1)))
			{
				for(const auto& f : mSubscribers[*i1])
				{
					f(*i1);
				}
			}
		}
	}
}

void Buffer::subscribeEvent(Event e, callback_t f)
{
	mSubscribers[e].push_back(f);
}

}}

