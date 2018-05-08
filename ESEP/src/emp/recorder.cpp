#include "emp/recorder.h"

namespace esep { namespace emp {

Recorder::Recorder(void)
{
	Physical::setCallback([this](Event e) {
		record(lib::Timer::instance().elapsed(), e);

		if(static_cast<bool>(mCallback))
		{
			mCallback(e);
		}
	});
}

void Recorder::record(uint64_t t, Event e)
{
}

}}
