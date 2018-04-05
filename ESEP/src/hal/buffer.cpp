#include "hal/buffer.h"

namespace esep { namespace hal {

void Buffer::update(Field f, uint32_t v)
{
	mFields[static_cast<uint>(f)] = v;
}

}}

