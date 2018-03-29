#include "hal/buffer.h"

namespace esep { namespace hal {

void Buffer::update(Field f, uint32_t v)
{
	if(f == Field::ANALOG)
	{
		mAnalog = static_cast<uint16_t>(v);
	}
	else
	{
		mPorts[static_cast<uint>(f)] = static_cast<uint8_t>(v);
	}
}

}}

