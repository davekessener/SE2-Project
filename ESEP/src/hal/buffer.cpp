#include "hal/buffer.h"

#include "lib/logger.h"

namespace esep { namespace hal {

void Buffer::update(Field f, uint32_t v)
{
	auto& r(mFields[static_cast<uint>(f)]);
	auto c = r ^ v;

//	MXT_LOG(lib::stringify("Updating buffer: old = ", lib::hex<32>(r), ", new = ", lib::hex<32>(v), " and diff = ", lib::hex<32>(c)));

	r = v;

	if(static_cast<bool>(mSubscriber))
	{
		switch(f)
		{
		case Field::GPIO_0:
			for(auto i1 = HAL::EVENTS, i2 = i1 + HAL::N_EVENTS ; i1 != i2 ; ++i1)
			{
				if((static_cast<uint64_t>(*i1) >> 32) == static_cast<uint>(f))
				{
					if(c & static_cast<uint>(*i1))
					{
						mSubscriber(*i1);
					}
				}
			}
			break;
		case Field::ANALOG:
			mSubscriber(Event::HEIGHT_SENSOR);
			break;
		default:
			break;
		}
	}
}

}}

