#include "hal/buffer.h"

#include "lib/logger.h"

namespace esep { namespace hal {

void Buffer::update(Field f, uint32_t v)
{
	auto& r(mFields[static_cast<uint>(f)]);
	auto c = r ^ v;

//	MXT_LOG(lib::stringify("Updating buffer: old = ", lib::hex<32>(r), ", new = ", lib::hex<32>(v), " and diff = ", lib::hex<32>(c)));

	r = v;

	if(f == Field::GPIO_0 && static_cast<bool>(mSubscriber))
	{
		for(auto i1 = HAL::EVENTS, i2 = i1 + HAL::N_EVENTS ; i1 != i2 ; ++i1)
		{
			if(c & (1 << static_cast<uint>(*i1)))
			{
				mSubscriber(*i1, v);
			}
		}
	}
}

}}

