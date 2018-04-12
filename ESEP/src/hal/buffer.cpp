#include "hal/buffer.h"

namespace esep { namespace hal {

void Buffer::update(Field f, uint32_t v)
{
	static const auto EVENTS = {
			Event::LB_START, Event::LB_END, Event::LB_HEIGHTSENSOR, Event::LB_RAMP, Event::LB_SWITCH,
			Event::BTN_START, Event::BTN_STOP, Event::BTN_RESET, Event::BTN_ESTOP };

	auto& r(mFields[static_cast<uint>(f)]);
	auto c = r ^ v;

	r = v;

	if(f == Field::GPIO_0)
	{
		for(const auto& e : EVENTS)
		{
			if(c & (1 << static_cast<uint>(e)))
			{
				for(const auto& f : mSubscribers[e])
				{
					f(e);
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

